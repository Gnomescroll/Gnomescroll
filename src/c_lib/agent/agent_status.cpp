#include "agent_status.hpp"

#include <math.h>

#if DC_SERVER
#include <chat/_interface.hpp>
#include <item/container/container.hpp>
#endif

/*
 * Agent_status has miscellaneous status properties (health, dead, ...)
 * Its methods are to be used by the server ONLY
 * The reflexive methods that will be called in the client are stored on Agent_event
 *
 * e.g.
 * Agent_status::die() // in server
 * Agent_event::died() // in client
 *
 */

const int VOXEL_MODEL_RESTORE_WAIT = 30 * 10; // ~ once every 10 seconds

Agent_status::Agent_status(Agent* a) :
    a(a),
    voxel_model_restore_throttle(0),
    health(AGENT_HEALTH),
    should_die(false),
    dead(true),
    respawn_countdown(RESPAWN_TICKS),
    spawner(BASE_SPAWN_ID),
    kills(0),
    deaths(0),
    suicides(0),
    slime_kills(0),
    health_max(AGENT_HEALTH),
    vox_crouched(false),
    lifetime(0),
    color(AGENT_DEFAULT_COLOR)
{
    this->name[0] = '\0';
}

Agent_status::~Agent_status()
{
    this->quit();
}

void Agent_status::set_spawner(int pt)
{
    ASSERT_VALID_SPAWNER_ID(pt);
    IF_INVALID_SPAWNER_ID(pt) return;

    if (pt == this->spawner) return;

    #if DC_SERVER
    using Components::AgentSpawnerComponent;
    if (pt != BASE_SPAWN_ID && this->spawner != BASE_SPAWN_ID)
    {   // transferring between spawners
        class Entities::Entity* old_spawner = Entities::get(OBJECT_AGENT_SPAWNER, this->spawner);
        GS_ASSERT(old_spawner != NULL);
        if (old_spawner != NULL)
        {
            AgentSpawnerComponent* agent_spawner = (AgentSpawnerComponent*)old_spawner->get_component(COMPONENT_AGENT_SPAWNER);
            GS_ASSERT(agent_spawner != NULL);
            if (agent_spawner != NULL) agent_spawner->remove_all(this->a->id);                
        }
    }
    #endif
    
    this->spawner = pt;

    #if DC_SERVER
    set_spawner_StoC msg;
    msg.spawner_id = pt;
    msg.sendToClient(this->a->client_id);

    if (pt == BASE_SPAWN_ID) return;

    class Entities::Entity* spawner = Entities::get(OBJECT_AGENT_SPAWNER, pt);
    GS_ASSERT(spawner != NULL);
    if (spawner == NULL) return;

    AgentSpawnerComponent* agent_spawner = (AgentSpawnerComponent*)spawner->get_component(COMPONENT_AGENT_SPAWNER);
    GS_ASSERT(agent_spawner != NULL);
    if (agent_spawner != NULL) agent_spawner->add_agent(this->a->id);

    // play sound
    // only send the sound if its not the base spawner set
    struct Vec3 pos;
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)spawner->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    GS_ASSERT(vox != NULL);
    if (vox == NULL)
    {
        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)spawner->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return;
        pos = physics->get_position();
    }
    else
        pos = vox->get_center();
        
    Sound::send_play_2d_sound("spawner_activate", this->a->client_id); // play sound as 2d for client
    Sound::broadcast_exclude_play_3d_sound("spawner_activate", pos, this->a->client_id);    // play 3d sound for everyone else
    #endif
}

#if DC_SERVER

void Agent_status::identify(const char* name)
{
    this->set_name(name);
}

void Agent_status::broadcast_color()
{
    agent_color_StoC msg;
    msg.agent_id = this->a->id;
    msg.color = this->color;
    msg.broadcast();
}

void Agent_status::send_color(ClientID client_id)
{
    agent_color_StoC msg;
    msg.agent_id = this->a->id;
    msg.color = this->color;
    msg.sendToClient(client_id);
}
#endif

bool Agent_status::set_color(struct Color color)
{
    if (!this->set_color_silent(color)) return false;
    #if DC_SERVER
    this->broadcast_color();
    #endif
    return true;
}

// does not broadcast the change (useful for the deserializer) 
bool Agent_status::set_color_silent(struct Color color)
{
    if (colors_equal(color, this->color)) return false;

    if (!color.r && !color.g && !color.b) color = color_init(1,1,1); // dont allow 0,0,0 (interpreted as empty voxel)
        
    this->color = color;
    
    #if DC_CLIENT
    this->a->event.color_changed = true;
    #endif

    // TODO -- REMOVE THIS HACK. have to fix something in the voxel color render pipeline
    // previous attempt failed -- not worth fixing for now
    // somewhere, somehow, 255 is rolling over
    // to 0 by the time it is rendered
    if (color.r == 255) this->color.r = 254;
    if (color.g == 255) this->color.g = 254;
    if (color.b == 255) this->color.b = 254;
    
    #if DC_SERVER
    this->a->vox->fill_color(this->color);
    #endif

    return true;
}

void Agent_status::set_name(const char* name)
{
    strcpy(this->name, name);
}

#if DC_SERVER
void Agent_status::heal(unsigned int amt)
{
    GS_ASSERT(amt > 0);
    if (this->dead || this->should_die) return;
    this->health += amt;
    if (this->health > (int)this->health_max)
        this->health = this->health_max;
}

int Agent_status::apply_damage(int dmg)
{
    GS_ASSERT(dmg >= 0);
    if (dmg <= 0) return this->health;

    if (this->dead || this->should_die) return this->health;
    
    agent_damage_StoC dmg_msg;
    dmg_msg.id = a->id;
    dmg_msg.dmg = dmg;
    dmg_msg.broadcast();

    if (this->health <= 0)
    {
        this->should_die = true;
        return this->health;
    }

    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;

    this->send_health_msg();

    if (this->health <= 0)
    {    // attempt to burn energy tank
        int energy_tanks_container_id = ItemContainer::get_agent_energy_tanks(this->a->id);
        GS_ASSERT(energy_tanks_container_id != NULL_CONTAINER);
        using ItemContainer::ItemContainerEnergyTanks;
        ItemContainerEnergyTanks* container = (ItemContainerEnergyTanks*)
                ItemContainer::get_container(energy_tanks_container_id);
        GS_ASSERT(container != NULL);
        int n_energy_tanks = 0;
        if (container != NULL)
            n_energy_tanks = container->consume_energy_tank();
        
        if (n_energy_tanks > 0)
            this->restore_health();
    }
        
    return this->health;
}

void Agent_status::send_health_msg()
{
    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.broadcast();
}

void Agent_status::send_health_msg(ClientID client_id)
{
    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.sendToClient(client_id);
}

int Agent_status::apply_damage(int dmg, AgentID inflictor_id, EntityType inflictor_type, int part_id)
{
    if (!Options::pvp)
    {   // dont allow player kills
        if ((inflictor_type == OBJECT_AGENT || inflictor_type == OBJECT_GRENADE)
          && inflictor_id != this->a->id) return this->health;
    }
    
    int health = this->apply_damage(dmg);
    AgentDeathMethod death_method = DEATH_NORMAL;
    if (inflictor_type == OBJECT_GRENADE)
        death_method = DEATH_GRENADE;
    else if (inflictor_type == OBJECT_TURRET)
        death_method = DEATH_TURRET;
    else if (inflictor_type == OBJECT_PLASMAGEN)
        death_method = DEATH_PLASMAGEN;
    else if (part_id == AGENT_PART_HEAD)
        death_method = DEATH_HEADSHOT;
        
    if (this->should_die)
        die(inflictor_id, inflictor_type, death_method);

    return health;
}

int Agent_status::apply_hitscan_laser_damage_to_part(int part_id, AgentID inflictor_id, EntityType inflictor_type)
{
    int dmg = 0;

    switch (part_id)
    {
        case AGENT_PART_HEAD:
            dmg = randrange(15,25);
            break;
        case AGENT_PART_TORSO:
            dmg = randrange(10,15);
            break;
        case AGENT_PART_LARM:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_RARM:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_LLEG:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_RLEG:
            dmg = randrange(5,10);
            break;
        default:
            GS_ASSERT(false);
            break;
    }
    return this->apply_damage(dmg, inflictor_id, inflictor_type, part_id);
}

int Agent_status::apply_mining_laser_damage_to_part(int part_id, AgentID inflictor_id, EntityType inflictor_type)
{
    int dmg = 0;

    switch (part_id)
    {
        case AGENT_PART_HEAD:
            dmg = randrange(15,25);
            break;
        case AGENT_PART_TORSO:
            dmg = randrange(10,15);
            break;
        case AGENT_PART_LARM:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_RARM:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_LLEG:
            dmg = randrange(5,10);
            break;
        case AGENT_PART_RLEG:
            dmg = randrange(5,10);
            break;
        default:
            GS_ASSERT(false);
            break;
    }
    dmg = ((float)dmg) * (1.0f/3.0f);
    if (dmg <= 0) dmg = 1;
    return this->apply_damage(dmg, inflictor_id, inflictor_type, part_id);
}

void Agent_status::set_fresh_state()
{
    a->spawn_state();
    this->lifetime = 0;
    this->restore_health();
    
    // revive
    this->dead = false;
    this->should_die = false;
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();

    ItemContainer::agent_born(this->a->id);
}

void Agent_status::respawn()
{
    if (!this->dead) return;  // ignore if not waiting to respawn
    
    respawn_countdown--;                  // decrement
    if (respawn_countdown > 0) return;  // abort if not ready
    this->set_fresh_state();
    respawn_countdown = RESPAWN_TICKS; // reset timer
}

void Agent_status::restore_health()
{
    if (this->health == AGENT_HEALTH) return;
    this->health = AGENT_HEALTH;
    this->send_health_msg();
}

void Agent_status::at_base()
{
    this->restore_health();
}

#endif

void Agent_status::quit()
{
    #if DC_SERVER
    if (this->spawner != BASE_SPAWN_ID)
    {
        class Entities::Entity* spawner = Entities::get(OBJECT_AGENT_SPAWNER, this->spawner);
        GS_ASSERT(spawner != NULL);
        if (spawner != NULL)
        {
            using Components::AgentSpawnerComponent;
            AgentSpawnerComponent* agent_spawner = (AgentSpawnerComponent*)spawner->get_component(COMPONENT_AGENT_SPAWNER);
            GS_ASSERT(agent_spawner != NULL);
            if (agent_spawner != NULL) agent_spawner->remove(this->a->id);                
        }
    }
    #endif
}

bool Agent_status::die()
{
    if (this->dead) return false;
    this->should_die = false;
    this->dead = true;
    this->deaths++;

    #if DC_SERVER
    AgentDeaths_StoC deaths_msg;
    deaths_msg.id = this->a->id;
    deaths_msg.deaths = this->deaths;
    deaths_msg.broadcast();
    
    agent_dead_StoC dead_msg;
    dead_msg.id = this->a->id;
    dead_msg.dead = this->dead;
    dead_msg.broadcast();

    Toolbelt::agent_died(this->a->id);
    ItemContainer::agent_died(this->a->id);
    #endif

    return true;
}

bool Agent_status::die(AgentID inflictor_id, EntityType inflictor_type, AgentDeathMethod death_method)
{
    bool killed = this->die();
    if (!killed) return false;
    
    Agent* attacker;
    //Turret* turret;
    switch (inflictor_type)
    {
        case OBJECT_GRENADE:
        case OBJECT_AGENT:
            attacker = Agents::get_agent(inflictor_id);
            if (attacker != NULL)
                attacker->status.kill(this->a->id);
            break;
        //case OBJECT_MONSTER_BOMB:
            //Monsters::Slime* slime = STATE::slime_list->get(inflictor_id);
            //if (slime != NULL) {}
            //break;
        //case OBJECT_TURRET:
            //turret = (Turret*)STATE::entity_list->get(inflictor_type, inflictor_id);
            //if (turret == NULL) break;
            //attacker = Agents::get_agent(turret->get_owner());
            //if (attacker != NULL)
                //attacker->status.kill(this->a->id);
            //break;
        default:
            //printf("Agent::die -- OBJECT %d not handled\n", inflictor_type);
            break;
    }

    #if DC_SERVER
    // send conflict notification to clients
    agent_conflict_notification_StoC msg;
    //Turret* turret;
    switch (inflictor_type)
    {
        case OBJECT_GRENADE:
        case OBJECT_AGENT:
            msg.victim = this->a->id;
            msg.attacker = inflictor_id;
            msg.method = death_method;    // put headshot, grenades here
            msg.broadcast();
            break;

        //case OBJECT_TURRET:
            //// lookup turret object, get owner, this will be the inflictor id
            //turret = (Turret*)ServerState::entity_list->get(inflictor_type, inflictor_id);
            //if (turret == NULL) break;
            //inflictor_id = turret->get_owner();
            //msg.victim = this->a->id;
            //msg.attacker = inflictor_id;
            //msg.method = death_method;    // put headshot, grenades here
            //msg.broadcast();
            //break;

        default: break;
    }
    #endif
    
    return true;
}

void Agent_status::kill(int victim_id)
{
    if (victim_id == this->a->id)
    {
        suicides++;
        AgentSuicides_StoC as;
        as.id = this->a->id;
        as.suicides = suicides;
        as.broadcast();
    }
    else
    {
        kills++;
        AgentKills_StoC ak;
        ak.id = this->a->id;
        ak.kills = kills;
        ak.broadcast();
    }
}

void Agent_status::kill_slime()
{
    this->slime_kills++;
}

void Agent_status::send_scores(ClientID client_id)
{
    AgentKills_StoC ak;
    ak.id = a->id;
    ak.kills = kills;
    ak.sendToClient(client_id);
    
    AgentDeaths_StoC ad;
    ad.id = a->id;
    ad.deaths = deaths;
    ad.sendToClient(client_id);

    AgentSuicides_StoC as;
    as.id = a->id;
    as.suicides = suicides;
    as.sendToClient(client_id);
}

// to all
void Agent_status::send_scores()
{
    AgentKills_StoC ak;
    ak.id = a->id;
    ak.kills = kills;
    ak.broadcast();
    
    AgentDeaths_StoC ad;
    ad.id = a->id;
    ad.deaths = deaths;
    ad.broadcast();

    AgentSuicides_StoC as;
    as.id = a->id;
    as.suicides = suicides;
    as.broadcast();
}

float Agent_status::get_spawn_angle()
{
    return 0.5f;
}

#if DC_SERVER
bool Agent_status::consume_item(ItemID item_id)
{
    int item_type = Item::get_item_type(item_id);
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return false;
    
    ItemGroup item_group = Item::get_item_group_for_type(item_type);
    GS_ASSERT(item_group == IG_CONSUMABLE);
    if (item_group != IG_CONSUMABLE) return false;

    Item::ItemAttribute* attr = Item::get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return false;

    static const int small_charge_pack = Item::get_item_type("small_charge_pack");

    if (item_type == small_charge_pack)
    {
        if (this->health >= (int)health_max) return false;
        GS_ASSERT(attr->repair_agent_amount > 0);
        this->heal(attr->repair_agent_amount);
        this->send_health_msg();
    }
    else
    {
        GS_ASSERT(false);
        return false;
    }
    return true;
}
#endif

void Agent_status::tick()
{
    if (this->dead || this->should_die)
        this->lifetime = 0;
    else
        this->lifetime++;
}
