#include "agent_status.hpp"

#include <math.h>

#if DC_SERVER
#include <chat/server.hpp>
#include <chat/interface.hpp>
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

const char AGENT_UNDEFINED_NAME[] = "undefined-agent-name";

Agent_status::Agent_status(Agent_state* a)
:
a(a),
voxel_model_restore_throttle(0),
health(AGENT_HEALTH),
should_die(false),
dead(true),
respawn_countdown(RESPAWN_TICKS),
spawner(BASE_SPAWN_ID),  // -1 will mean default spawn point (base)
kills(0),
deaths(0),
suicides(0),
slime_kills(0),
health_max(AGENT_HEALTH),
identified(false),
vox_crouched(false),
lifetime(0),
color_chosen(false)
#if DC_SERVER
, net_peer_ready(false)
#endif
{
    color.r=color.g=color.b=48;
    strcpy(this->name, AGENT_UNDEFINED_NAME);
}

Agent_status::~Agent_status()
{
}

void Agent_status::set_spawner(int pt)
{
    GS_ASSERT(pt == BASE_SPAWN_ID || (pt >= 0 && pt <= 0xffff));
    this->spawner = pt;
    #if DC_SERVER
    set_spawner_StoC msg;
    msg.spawner_id = pt;
    msg.sendToClient(this->a->id);
    #endif
}

#if DC_SERVER
void Agent_status::broadcast_color()
{
    agent_color_StoC msg;
    msg.r = this->color.r;
    msg.g = this->color.g;
    msg.b = this->color.b;
    msg.agent_id = this->a->id;
    msg.broadcast();
}

void Agent_status::send_color(int client_id)
{
    agent_color_StoC msg;
    msg.r = this->color.r;
    msg.g = this->color.g;
    msg.b = this->color.b;
    msg.agent_id = this->a->id;
    msg.sendToClient(client_id);
}
#endif

void Agent_status::set_color(struct Color color)
{
    if (this->color_chosen && colors_equal(color, this->color)) return;
        
    this->color = color;
    this->color_chosen = true;
    
    #if DC_CLIENT
    this->a->event.color_changed = true;
    #endif

    #if DC_SERVER
    this->broadcast_color();
    #endif

    // TODO -- REMOVE THIS HACK
    // somewhere, somehow, 255 is rolling over
    // to 0 by the time it is rendered
    if (color.r == 255) color.r = 254;
    if (color.g == 255) color.g = 254;
    if (color.b == 255) color.b = 254;
    
    #if DC_SERVER
    this->a->vox->fill_color(this->color);
    #endif
}

bool Agent_status::set_name(char* name)
{
    #if DC_SERVER
    if (strcmp(AGENT_UNDEFINED_NAME, name) == 0)    // cant be the undefined holder
        return false;
    if (name[0] == '\0')                            // no empties
        return false;
    #endif
    
    if (strlen(name) > PLAYER_NAME_MAX_LENGTH)
        name[PLAYER_NAME_MAX_LENGTH-1] = '\0';

    bool new_name = (strcmp(this->name, name) == 0) ? false : true;

    strcpy(this->name, name);
    #if DC_SERVER
    agent_name_StoC msg;
    msg.id = this->a->id;
    strcpy(msg.name, this->name);
    msg.broadcast();
    #endif

    return new_name;
}

#if DC_CLIENT
void Agent_status::check_missing_name()
{
    if (strcmp(name, AGENT_UNDEFINED_NAME) == 0)
    {
        request_agent_name_CtoS msg;
        msg.id = this->a->id;
        msg.send();
    }
}
#endif

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

void Agent_status::send_health_msg(int client_id)
{
    agent_health_StoC health_msg;
    health_msg.id = a->id;
    health_msg.health = this->health;
    health_msg.sendToClient(client_id);
}

int Agent_status::apply_damage(int dmg, int inflictor_id, ObjectType inflictor_type, int part_id)
{
    if (!Options::pvp)
    {   // dont allow player kills
        if ((inflictor_type == OBJECT_AGENT || inflictor_type == OBJECT_GRENADE)
          && inflictor_id != this->a->id) return this->health;
    }
    
    int health = this->apply_damage(dmg);
    AgentDeathMethod death_method = DEATH_NORMAL;
    if (part_id == AGENT_PART_HEAD)
        death_method = DEATH_HEADSHOT;
    else if (inflictor_type == OBJECT_GRENADE)
        death_method = DEATH_GRENADE;
    else if (inflictor_type == OBJECT_TURRET)
        death_method = DEATH_TURRET;
        
    if (this->should_die)
        die(inflictor_id, inflictor_type, death_method);

    return health;
}

int Agent_status::apply_hitscan_laser_damage_to_part(int part_id, int inflictor_id, ObjectType inflictor_type)
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

int Agent_status::apply_mining_laser_damage_to_part(int part_id, int inflictor_id, ObjectType inflictor_type)
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
    if (!this->net_peer_ready)
        dead_msg.sendToClient(this->a->client_id);

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

bool Agent_status::die(int inflictor_id, ObjectType inflictor_type, AgentDeathMethod death_method)
{
    bool killed = this->die();
    if (!killed) return false;
    
    Agent_state* attacker;
    //Turret* turret;
    switch (inflictor_type)
    {
        case OBJECT_GRENADE:
        case OBJECT_AGENT:
            attacker = STATE::agent_list->get(inflictor_id);
            if (attacker != NULL)
                attacker->status.kill(this->a->id);
            break;
        //case OBJECT_MONSTER_BOMB:
            //Monsters::Slime* slime = STATE::slime_list->get(inflictor_id);
            //if (slime != NULL) {}
            //break;
        //case OBJECT_TURRET:
            //turret = (Turret*)STATE::object_list->get(inflictor_type, inflictor_id);
            //if (turret == NULL) break;
            //attacker = STATE::agent_list->get(turret->get_owner());
            //if (attacker != NULL)
                //attacker->status.kill(this->a->id);
            //break;
        default:
            //printf("Agent_state::die -- OBJECT %d not handled\n", inflictor_type);
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
            //turret = (Turret*)ServerState::object_list->get(inflictor_type, inflictor_id);
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

void Agent_status::send_scores(int client_id)
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
