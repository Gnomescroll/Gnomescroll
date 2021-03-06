/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "agent_status.hpp"

#include <math.h>

#if DC_SERVER
# include <chat/_interface.hpp>
# include <item/container/container.hpp>
#endif

namespace Agents
{

/*
 * AgentStatus has miscellaneous status properties (health, dead, ...)
 * Its methods are to be used by the server ONLY
 * The reflexive methods that will be called in the client are stored on AgentEvent
 *
 * e.g.
 * AgentStatus::die() // in server
 * AgentEvent::died() // in client
 *
 */

const int VOXEL_MODEL_RESTORE_WAIT = 30 * 10; // ~ once every 10 seconds

AgentStatus::AgentStatus(Agent* a) :
    hunger_tick(0),
    hunger_regen_tick(0),
    hunger_damage_tick(0),
    should_die(false),
    dead(true),
    respawn_countdown(RESPAWN_TICKS),
    spawner(BASE_SPAWN_ID),
    kills(0),
    deaths(0),
    suicides(0),
    slime_kills(0),
    n_badges(0),
    vox_crouched(false),
    lifetime(0),
    color(AGENT_DEFAULT_COLOR),
    a(a),
    voxel_model_restore_throttle(0)
{
    this->name[0] = '\0';

    for (size_t i=0; i<PLAYER_MAX_BADGES; i++)
        this->badges[i] = NULL_BADGE;

    #if DC_SERVER && !PRODUCTION
    if (rand()%2) this->add_badge(Badges::get_badge_type("debug"));
    #endif
}

AgentStatus::~AgentStatus()
{
    this->quit();
}

void AgentStatus::set_spawner(EntityID pt)
{
    ASSERT_VALID_SPAWNER_ID(pt);
    IF_INVALID_SPAWNER_ID(pt) return;

    if (pt == this->spawner) return;

    #if DC_SERVER
    if (pt != BASE_SPAWN_ID && this->spawner != BASE_SPAWN_ID)
    {   // transferring between spawners
        class Entities::Entity* old_spawner = Entities::get(ENTITY_AGENT_SPAWNER, this->spawner);
        GS_ASSERT(old_spawner != NULL);
        if (old_spawner != NULL)
        {
            auto agent_spawner = GET_COMPONENT(AgentSpawner, old_spawner);
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

    class Entities::Entity* spawner = Entities::get(ENTITY_AGENT_SPAWNER, pt);
    IF_ASSERT(spawner == NULL) return;

    auto agent_spawner = GET_COMPONENT(AgentSpawner, spawner);
    GS_ASSERT(agent_spawner != NULL);
    if (agent_spawner != NULL) agent_spawner->add_agent(this->a->id);

    // play sound
    // only send the sound if its not the base spawner set
    struct Vec3 pos;
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, spawner);
    IF_ASSERT(vox == NULL)
    {
        auto physics = GET_COMPONENT_INTERFACE(Physics, spawner);
        IF_ASSERT(physics == NULL) return;
        pos = physics->get_position();
    }
    else
        pos = vox->get_center();

    Sound::send_play_2d_sound("spawner_activate", this->a->client_id); // play sound as 2d for client
    Sound::broadcast_exclude_play_3d_sound("spawner_activate", pos, this->a->client_id);    // play 3d sound for everyone else
    #endif
}

bool AgentStatus::set_color(Color color)
{
    if (!this->set_color_silent(color)) return false;
    #if DC_SERVER
    this->broadcast_color();
    #endif
    return true;
}

void AgentStatus::add_badge(BadgeType badge_id)
{
    IF_ASSERT(this->n_badges >= PLAYER_MAX_BADGES) return;
    IF_ASSERT(!isValid(badge_id)) return;

    // no dupes
    for (size_t i=0; i<this->n_badges; i++)
        GS_ASSERT(this->badges[i] != badge_id);

    this->badges[this->n_badges++] = badge_id;
}

// does not broadcast the change (useful for the deserializer)
bool AgentStatus::set_color_silent(Color color)
{
    if (colors_equal(color, this->color)) return false;

    if (!color.r && !color.g && !color.b) color = Color(1,1,1); // dont allow 0,0,0 (interpreted as empty voxel)

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

void AgentStatus::set_name(const char* name)
{
    strcpy(this->name, name);
}

void AgentStatus::quit()
{
    #if DC_SERVER
    if (this->spawner != BASE_SPAWN_ID)
    {
        class Entities::Entity* spawner = Entities::get(ENTITY_AGENT_SPAWNER, this->spawner);
        GS_ASSERT(spawner != NULL);
        if (spawner != NULL)
        {
            auto agent_spawner = GET_COMPONENT(AgentSpawner, spawner);
            GS_ASSERT(agent_spawner != NULL);
            if (agent_spawner != NULL) agent_spawner->remove(this->a->id);
        }
    }
    #endif
}

bool AgentStatus::die()
{
    if (this->dead) return false;
    this->should_die = false;
    this->dead = true;
    this->deaths++;

    #if DC_SERVER
    agent_deaths_StoC deaths_msg;
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

bool AgentStatus::die(AgentID inflictor_id, EntityType inflictor_type, AgentDeathMethod death_method)
{
    bool killed = this->die();
    if (!killed) return false;

    if (inflictor_id != NULL_AGENT)
    {
        Agent* attacker = Agents::get_agent(inflictor_id);
        if (attacker != NULL)
            attacker->status.kill(this->a->id);

        #if DC_SERVER
        agent_conflict_notification_StoC msg;
        msg.victim = this->a->id;
        msg.attacker = inflictor_id;
        msg.method = death_method;    // put headshot, grenades here
        msg.broadcast();
        #endif
    }

    return true;
}

void AgentStatus::kill(AgentID victim_id)
{
    if (victim_id == this->a->id)
    {
        suicides++;
        agent_suicides_StoC as;
        as.id = this->a->id;
        as.suicides = suicides;
        as.broadcast();
    }
    else
    {
        kills++;
        agent_kills_StoC ak;
        ak.id = this->a->id;
        ak.kills = kills;
        ak.broadcast();
    }
}

void AgentStatus::kill_slime()
{
    this->slime_kills++;
}

float AgentStatus::get_spawn_angle()
{
    return 0.5f;
}

void AgentStatus::tick()
{
    if (this->dead || this->should_die)
        this->lifetime = 0;
    else
        this->lifetime++;
}

#if DC_SERVER
void AgentStatus::identify(const char* name)
{
    this->set_name(name);
}

void AgentStatus::broadcast_color()
{
    agent_color_StoC msg;
    msg.agent_id = this->a->id;
    msg.color = this->color;
    msg.broadcast();
}

void AgentStatus::send_color(ClientID client_id)
{
    agent_color_StoC msg;
    msg.agent_id = this->a->id;
    msg.color = this->color;
    msg.sendToClient(client_id);
}

int AgentStatus::heal(unsigned int amt)
{
    if (this->dead || this->should_die) return 0;
    int health = get_attribute_int(this->a->id, "health");
    health += amt;
    set_attribute(this->a->id, "health", health);
    return get_attribute_int(this->a->id, "health");
}

int AgentStatus::hurt(unsigned int amt)
{
    if (this->dead || this->should_die) return 0;
    int health = get_attribute_int(this->a->id, "health");
    health -= amt;
    if (health <= 0) this->should_die = true;
    set_attribute(this->a->id, "health", health);
    return get_attribute_int(this->a->id, "health");
}

void AgentStatus::tick_rad()
{
    //printf("tick rad\n");
    this->rad_damage_tick++;

    struct Vec3 p = a->get_position();
    p = translate_position(vec3_add(p, vec3_init(0.0f, 0.0f, 0.5f)));
    int rad_level = t_map::get_rad_level(vec3i_init(p));
    set_attribute(this->a->id, "rad_level", rad_level);

    int rad_exposure = get_attribute_int(this->a->id, "rad_exposure");

    int _rad_level = 0; //tiers of rad exposure
    if( rad_level != 0)
    {
        if(rad_level < 4)
        {
            _rad_level = 1;
            rad_exposure += RAD_EXPOSURE_LEVEL1;
        }
        else if (rad_level < 8)
        {
            _rad_level = 2;
            rad_exposure += RAD_EXPOSURE_LEVEL2;
        }
        else if (rad_level < 12)
        {
            _rad_level = 3;
            rad_exposure += RAD_EXPOSURE_LEVEL3;
        }
        else
        {
            _rad_level = 4;
            rad_exposure += RAD_EXPOSURE_LEVEL4;
        }
    }

    if(_rad_level == 0)
        rad_exposure -= RAD_LEACH_RATE;

    set_attribute(this->a->id, "rad_exposure", rad_exposure);

    //calculate damage
    rad_exposure = GS_MAX(1, rad_exposure);
    int rad_exposure_level = ((4*rad_exposure)-1) / RAD_EXPOSURE_MAX;
    GS_ASSERT(rad_exposure_level >= 0 && rad_exposure_level <= 3);
    switch(rad_exposure_level)
    {
        case 0:
            break;
        case 1:
            if(this->rad_damage_tick % RAD_DAMAGE_RATE_LEVEL1 == 0)
                this->hurt(1);
            break;
        case 2:
            if(this->rad_damage_tick % RAD_DAMAGE_RATE_LEVEL2 == 0)
                this->hurt(1);
            break;
        case 3:
            if(this->rad_damage_tick % RAD_DAMAGE_RATE_LEVEL3 == 0)
                this->hurt(1);
            break;
        default:
            GS_ASSERT(false);
            break;
    }
}

void AgentStatus::tick_hunger()
{
    if (this->dead || this->should_die) return;
    int hunger = get_attribute_int(this->a->id, "hunger");
    if (hunger <= HUNGER_REGEN_THRESHOLD)
    {
        if (++this->hunger_regen_tick % HUNGER_REGEN_RATE == 0)
            this->heal(HUNGER_REGEN_AMOUNT);
    }
    else
        this->hunger_regen_tick = 0;

    if (hunger >= HUNGER_DAMAGE_THRESHOLD)
    {
        if (++this->hunger_damage_tick % HUNGER_DAMAGE_RATE == 0)
        {
            this->hurt(HUNGER_DAMAGE_AMOUNT);
            if (this->should_die)
                this->die(this->a->id, ENTITY_AGENT, DEATH_STARVATION);
        }
    }
    else
        this->hunger_damage_tick = 0;

    if (++this->hunger_tick % HUNGER_INCREASE_RATE) return;
    hunger += 1;
    set_attribute(this->a->id, "hunger", hunger);
}

int AgentStatus::apply_damage(int dmg)
{
    int health = get_attribute_int(this->a->id, "health");
    IF_ASSERT(dmg < 0) return health;
    if (dmg == 0) return health;
    if (this->dead || this->should_die) return health;

    ItemContainerID energy_tanks_container_id = ItemContainer::get_agent_energy_tanks(this->a->id);
    GS_ASSERT(energy_tanks_container_id != NULL_CONTAINER);
    using ItemContainer::ItemContainerEnergyTanks;
    ItemContainerEnergyTanks* container = (ItemContainerEnergyTanks*)
            ItemContainer::get_container(energy_tanks_container_id);
    GS_ASSERT(container != NULL);
    int remaining_dmg = dmg;
    if (container != NULL)
        remaining_dmg = container->consume_energy_tanks(dmg);

    return this->hurt(remaining_dmg);
}

int AgentStatus::apply_damage(int dmg, AgentID inflictor_id, EntityType inflictor_type, int part_id)
{
    if (!Options::pvp)
    {   // dont allow player kills
        if ((inflictor_type == ENTITY_AGENT ||
             inflictor_type == ENTITY_GRENADE) &&
            inflictor_id != this->a->id)
        {
            return get_attribute_int(this->a->id, "health");
        }
    }

    AgentDeathMethod death_method = DEATH_NORMAL;
    if (inflictor_type == ENTITY_GRENADE)
        death_method = DEATH_GRENADE;
    else if (inflictor_type == ENTITY_TURRET)
        death_method = DEATH_TURRET;
    else if (inflictor_type == ENTITY_PLASMAGEN)
        death_method = DEATH_PLASMAGEN;
    else if (part_id == AGENT_PART_HEAD)
        death_method = DEATH_HEADSHOT;

    return this->apply_damage(dmg, inflictor_id, inflictor_type, death_method);
}

int AgentStatus::apply_damage(int dmg, EntityType inflictor_type)
{
    return this->apply_damage(dmg, NULL_AGENT, inflictor_type);
}

int AgentStatus::apply_damage(int dmg, AgentID inflictor_id, EntityType inflictor_type)
{
    return this->apply_damage(dmg, inflictor_id, inflictor_type, NULL_AGENT_PART);
}

int AgentStatus::apply_damage(int dmg, AgentID inflictor_id, EntityType inflictor_type, AgentDeathMethod death_method)
{
    int health = this->apply_damage(dmg);
    if (this->should_die)
        this->die(inflictor_id, inflictor_type, death_method);
    return health;
}

int AgentStatus::apply_damage(int dmg, AgentDeathMethod death_method)
{   // assumes self-inflicted wound
    return this->apply_damage(dmg, this->a->id, ENTITY_AGENT, death_method);
}

void AgentStatus::set_fresh_state()
{
    a->spawn_state();
    this->lifetime = 0;
    this->restore_health();
    this->hunger_damage_tick = 0;
    this->hunger_regen_tick = 0;
    this->hunger_tick = 0;
    this->rad_damage_tick = 0;
    // TODO -- attributes should be labelled as stateful or not
        // Stateful etc attributes are restored to default on birth;
    int health_max = get_attribute_int(this->a->id, "health_max");
    set_attribute(this->a->id, "health", health_max);
    set_attribute(this->a->id, "hunger", 0);

    // revive
    this->dead = false;
    this->should_die = false;
    agent_dead_StoC dead_msg;
    dead_msg.id = a->id;
    dead_msg.dead = dead;
    dead_msg.broadcast();

    ItemContainer::agent_born(this->a->id);
}

void AgentStatus::respawn()
{
    if (!this->dead) return;  // ignore if not waiting to respawn

    respawn_countdown--;                  // decrement
    if (respawn_countdown > 0) return;  // abort if not ready
    this->set_fresh_state();
    respawn_countdown = RESPAWN_TICKS; // reset timer
}

void AgentStatus::restore_health()
{
    int health_max = get_attribute_int(this->a->id, "health_max");
    set_attribute(this->a->id, "health", health_max);
}

void AgentStatus::at_base()
{
    //this->restore_health();
}

void AgentStatus::send_scores(ClientID client_id)
{
    agent_kills_StoC ak;
    ak.id = a->id;
    ak.kills = kills;
    ak.sendToClient(client_id);

    agent_deaths_StoC ad;
    ad.id = a->id;
    ad.deaths = deaths;
    ad.sendToClient(client_id);

    agent_suicides_StoC as;
    as.id = a->id;
    as.suicides = suicides;
    as.sendToClient(client_id);
}

void AgentStatus::send_scores()
{
    agent_kills_StoC ak;
    ak.id = a->id;
    ak.kills = kills;
    ak.broadcast();

    agent_deaths_StoC ad;
    ad.id = a->id;
    ad.deaths = deaths;
    ad.broadcast();

    agent_suicides_StoC as;
    as.id = a->id;
    as.suicides = suicides;
    as.broadcast();
}

void AgentStatus::send_badges(ClientID client_id)
{
    for (size_t i=0; i<this->n_badges; i++)
        Badges::send_badge(this->badges[i], this->a->id, client_id);
}

void AgentStatus::broadcast_badges()
{
    for (size_t i=0; i<this->n_badges; i++)
        Badges::broadcast_badge(this->badges[i], this->a->id);
}

bool AgentStatus::consume_item(ItemID item_id)
{
    ItemType item_type = Item::get_item_type(item_id);
    IF_ASSERT(item_type == NULL_ITEM_TYPE) return false;
    ItemGroup item_group = Item::get_item_group_for_type(item_type);
    IF_ASSERT(item_group != IG_CONSUMABLE) return false;
    Item::ItemAttribute* attr = Item::get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return false;
    const ModifierList* modifiers = Item::get_item_modifiers(item_type);
    IF_ASSERT(modifiers == NULL) return false;

    bool applied = false;
    for (size_t i=0; i<modifiers->max; i++)
        if (modifiers->objects[i].id != modifiers->null_id)
        {
            bool _applied = apply_agent_modifier(this->a->id, &modifiers->objects[i]);
            if (_applied) applied = true;
        }
    return applied;
}
#endif  // DC_SERVER

}   // Agents
