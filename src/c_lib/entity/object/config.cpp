#include "config.hpp"

namespace Entities
{

static class EntityConfig* object_conf = NULL;

class EntityConfig
{
    public:
        bool loaded;

        EntityType type;
        unsigned int max;
        entityLoad   loader;
        entityReady  ready;
        entityDie    die;
        bool networked;

        tickEntity tick;
        updateEntity update;

        CreatePacketDelegate* create_packet;
        StatePacketDelegate* state_packet;

    EntityConfig()
    {
        this->init();
    }

    void init()
    {
        this->type = NULL_ENTITY_TYPE;
        this->max = 0;
        this->loader = NULL;
        this->ready = NULL;
        this->die = NULL;
        this->tick = NULL;
        this->update = NULL;
        this->create_packet = NULL;
        this->state_packet = NULL;
        this->networked = true;
        this->loaded = false;
    }
};

static class EntityConfig c;
static bool started = false;

static void commit_object_config()
{
    GS_ASSERT(c.type != NULL_ENTITY_TYPE);

    // no duplicate types
    GS_ASSERT(!object_conf[c.type].loaded);

    c.loaded = true;
    object_conf[c.type] = c;
    c.init();
}

static void set_object(EntityType type)
{
    if (started) commit_object_config();
    started = true;

    c.type = type;
}

static void register_settings()
{
    GS_ASSERT(!started);

    // fabs

    set_object(ENTITY_AGENT_SPAWNER);
    c.loader = &load_agent_spawner_data;
    c.ready = &ready_agent_spawner;
    c.die = &die_agent_spawner;
    c.max = MAX_SPAWNERS;
    c.tick = &tick_agent_spawner;
    c.update = &update_agent_spawner;
    c.create_packet = create_packet;
    c.state_packet = state_packet;

    set_object(ENTITY_BASE);
    c.loader = &load_base_data;
    c.ready = &ready_base;
    c.die = &die_base;
    c.max = 2;
    c.tick = &tick_base;
    c.update = &update_base;
    c.create_packet = create_packet;
    c.state_packet = state_packet;

    set_object(ENTITY_TURRET);
    c.loader = &load_turret_data;
    c.ready = &ready_turret;
    c.die = &die_turret;
    c.max = 512;
    c.tick = &tick_turret;
    c.update = &update_turret;
    c.create_packet = create_packet_owner;
    c.state_packet = state_packet;

    set_object(ENTITY_ENERGY_CORE);
    c.loader = &load_energy_core_data;
    c.ready = &ready_energy_core;
    c.die = &die_energy_core;
    c.max = 1024;
    c.tick = &tick_energy_core;
    c.update = &update_energy_core;
    c.create_packet = create_packet;
    c.state_packet = state_packet;

    // mobs

    set_object(ENTITY_MONSTER_SPAWNER);
    c.loader = &load_mob_spawner_data;
    c.ready = &ready_mob_spawner;
    c.die = &die_mob_spawner;
    c.max = 64;
    c.tick = &tick_mob_spawner;
    c.update = &update_mob_spawner;
    c.create_packet = create_packet;
    c.state_packet = state_packet;

    set_object(ENTITY_MONSTER_BOX);
    c.loader = &load_mob_robot_box_data;
    c.ready = &ready_mob_robot_box;
    c.die = &die_mob_robot_box;
    c.max = 512;
    c.tick = &tick_mob_robot_box;
    c.update = &update_mob_robot_box;
    c.create_packet = create_packet_momentum_angles_health;
    c.state_packet = state_packet_momentum_angles;

    set_object(ENTITY_MONSTER_BOMB);
    c.loader = &load_mob_bomb_data;
    c.ready = &ready_mob_bomb;
    c.die = &die_mob_bomb;
    c.max = 256;
    c.tick = &tick_mob_bomb;
    c.update = &update_mob_bomb;
    c.create_packet = create_packet_momentum_angles_health;
    c.state_packet = state_packet_momentum_angles;

    set_object(ENTITY_MONSTER_SLIME);
    c.loader = &load_mob_slime_data;
    c.ready = &ready_mob_slime;
    c.die = &die_mob_slime;
    c.max = 512;
    c.tick = &tick_mob_slime;
    c.update = &update_mob_slime;
    c.create_packet = create_packet_momentum_angles_health;
    c.state_packet = state_packet_momentum_angles;

    set_object(ENTITY_MONSTER_LIZARD_THIEF);
    c.loader = &load_mob_lizard_thief_data;
    c.ready = &ready_mob_lizard_thief;
    c.die = &die_mob_lizard_thief;
    c.max = 128;
    c.tick = &tick_mob_lizard_thief;
    c.update = &update_mob_lizard_thief;
    c.create_packet = create_packet_momentum_angles_health;
    c.state_packet = state_packet_momentum_angles;

    set_object(ENTITY_MONSTER_BLUE_BLUB);
    c.loader = &load_mob_blub_data;
    c.ready = &ready_mob_blub;
    c.die = &die_mob_blub;
    c.max = 128;
    c.tick = &tick_mob_blub;
    c.update = &update_mob_blub;
    c.create_packet = create_packet_momentum_angles_health;
    c.state_packet = state_packet_momentum_angles;

    commit_object_config();
}

static void validate_settings()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {   // either everything is set or nothing
        EntityConfig* c = &object_conf[i];
        GS_ASSERT((c->max    != 0 &&
                   c->loader != NULL &&
                   c->ready  != NULL &&
                   c->tick   != NULL &&
                   c->update != NULL &&
                   c->die    != NULL) ||
                  (c->max    == 0 &&
                   c->loader == NULL &&
                   c->ready  == NULL &&
                   c->die    == NULL));

        GS_ASSERT(!c->networked ||
                  (c->state_packet != NULL && c->create_packet != NULL));
    }
}

void init_config()
{
    GS_ASSERT(object_conf == NULL);
    object_conf = new EntityConfig[MAX_ENTITY_TYPES];

    register_settings();
    validate_settings();
}

void teardown_config()
{
    delete[] object_conf;
}

size_t get_entity_max(EntityType type)
{
    IF_ASSERT(!isValid(type)) return 0;
    return object_conf[type].max;
}

entityLoad get_entity_load_method(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return object_conf[type].loader;
}

entityReady get_entity_ready_method(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return object_conf[type].ready;
}

entityDie get_entity_die_method(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return object_conf[type].die;
}

entityTick get_entity_tick_method(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return object_conf[type].tick;
}

entityUpdate get_entity_update_method(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return object_conf[type].update;
}

CreatePacketDelegate* get_entity_create_packet_delegate(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return object_conf[type].create_packet;
}

StatePacketDelegate* get_entity_state_packet_delegate(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return object_conf[type].state_packet;
}

bool object_is_networked(EntityType type)
{
    IF_ASSERT(!isValid(type)) return false;
    return object_conf[type].networked;
}

}   // Entities
