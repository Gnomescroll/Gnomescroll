#include "config.hpp"

#include <common/dat/properties.hpp>
#include <common/dat/name_map.hpp>

namespace Entities
{

static class EntityProperties* entity_conf = NULL;
static class DatNameMap* entity_name_map = NULL;

class EntityProperty: public Property<EntityType>
{
    public:
        size_t max;
        entityLoad loader;
        CreatePacketDelegate* create_packet;
        StatePacketDelegate* state_packet;
        bool networked;

    EntityProperty() :
        Property<EntityType>(NULL_ENTITY_TYPE),
        max(0), loader(NULL), create_packet(NULL),
        state_packet(NULL), networked(false)
    {}
};

class EntityProperties: public Properties<EntityProperty, EntityType>
{
    public:

    EntityProperties() :
        Properties<EntityProperty, EntityType>(MAX_ENTITY_TYPES)
    {}
};


static class EntityProperty* c = NULL;

static void set_entity(EntityType type, const char* name)
{
    c = entity_conf->get_next(type);
    c->set_name(name);
    c->networked = true;  // everything defined should be networked by default
}

static void register_settings()
{
    GS_ASSERT(c == NULL);

    // fabs
    set_entity(ENTITY_AGENT_SPAWNER, "agent_spawner");
    c->loader = &load_agent_spawner_data;
    c->max = MAX_SPAWNERS;
    c->create_packet = create_packet;
    c->state_packet = state_packet;

    set_entity(ENTITY_BASE, "base");
    c->loader = &load_base_data;
    c->max = 2;
    c->create_packet = create_packet;
    c->state_packet = state_packet;

    set_entity(ENTITY_TURRET, "turret");
    c->loader = &load_turret_data;
    c->max = 512;
    c->create_packet = create_packet_owner;
    c->state_packet = state_packet;

    set_entity(ENTITY_ENERGY_CORE, "energy_core");
    c->loader = &load_energy_core_data;
    c->max = 512;
    c->create_packet = create_packet;
    c->state_packet = state_packet;

    // mobs
    set_entity(ENTITY_MONSTER_SPAWNER, "monster_spawner");
    c->loader = &load_mob_spawner_data;
    c->max = 64;
    c->create_packet = create_packet;
    c->state_packet = state_packet;

    set_entity(ENTITY_MONSTER_BOX, "box");
    c->loader = &load_mob_robot_box_data;
    c->max = 512;
    c->create_packet = create_packet_momentum_angles_health;
    c->state_packet = state_packet_momentum_angles;

    set_entity(ENTITY_MONSTER_BOMB, "bomb");
    c->loader = &load_mob_bomb_data;
    c->max = 256;
    c->create_packet = create_packet_momentum_angles_health;
    c->state_packet = state_packet_momentum_angles;

    set_entity(ENTITY_MONSTER_SLIME, "slime");
    c->loader = &load_mob_slime_data;
    c->max = 512;
    c->create_packet = create_packet_momentum_angles_health;
    c->state_packet = state_packet_momentum_angles;

    set_entity(ENTITY_MONSTER_LIZARD_THIEF, "lizard_thief");
    c->loader = &load_mob_lizard_thief_data;
    c->max = 128;
    c->create_packet = create_packet_momentum_angles_health;
    c->state_packet = state_packet_momentum_angles;

    set_entity(ENTITY_MONSTER_BLUE_BLUB, "blub");
    c->loader = &load_mob_blub_data;
    c->max = 128;
    c->create_packet = create_packet_momentum_angles_health;
    c->state_packet = state_packet_momentum_angles;

    entity_conf->done_loading();
    entity_conf->set_pretty_names();
}

static void change_entity(const char* original, const char* replacement)
{   // Use this to remove or rename an entity
    GS_ASSERT_ABORT(is_valid_name(original));
    GS_ASSERT_ABORT(is_valid_name(replacement));
    bool mapped = entity_name_map->add_definition(original, replacement);
    GS_ASSERT_ABORT(mapped);
}

static void apply_entity_dat_changes()
{
    // example:
    //change_entity("blub", "glob");
}

static void validate_settings()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {   // either everything is set or nothing
        EntityProperty* c = &entity_conf->properties[i];
        if (!c->loaded) continue;
        GS_ASSERT((c->max    != 0 &&
                   c->loader != NULL) |
                  (c->max    == 0 &&
                   c->loader == NULL))

        GS_ASSERT(!c->networked ||
                  (c->state_packet != NULL && c->create_packet != NULL));
    }
}

#define ENTITY_NAME_FILE_ACTIVE   "entity_names.active"
#define ENTITY_NAME_FILE_INACTIVE "entity_names.inactive"

static void save_entity_names()
{
    #if DC_SERVER || !PRODUCTION
    bool saved = save_active_names(entity_conf->properties,
        MAX_ENTITY_TYPES, DAT_NAME_MAX_LENGTH, DATA_PATH ENTITY_NAME_FILE_ACTIVE);
    GS_ASSERT_ABORT(saved);
    saved = entity_name_map->save(DATA_PATH ENTITY_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT(saved);
    #endif
}

void init_config()
{
    GS_ASSERT(entity_conf == NULL);
    GS_ASSERT(entity_name_map == NULL);
    entity_conf = new EntityProperties;
    entity_name_map = new DatNameMap(256, DAT_NAME_MAX_LENGTH);
    register_settings();
    apply_entity_dat_changes();
    validate_settings();
    save_entity_names();
}

void teardown_config()
{
    delete entity_conf;
    delete entity_name_map;
}

static EntityProperty* get_entity_property(EntityType type)
{
    return entity_conf->get(type);
}

static EntityProperty* get_entity_property(const char* name)
{
    return entity_conf->get(name);
}

size_t get_entity_max(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    if (attr == NULL) return 0;
    return attr->max;
}

entityLoad get_entity_load_method(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    if (attr == NULL) return NULL;
    return attr->loader;
}

CreatePacketDelegate* get_entity_create_packet_delegate(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    if (attr == NULL) return NULL;
    return attr->create_packet;
}

StatePacketDelegate* get_entity_state_packet_delegate(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    if (attr == NULL) return NULL;
    return attr->state_packet;
}

bool entity_is_networked(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    if (attr == NULL) return false;
    return attr->networked;
}

const char* get_entity_name(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    IF_ASSERT(attr == NULL) return NULL;
    return attr->name;
}

const char* get_entity_pretty_name(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    IF_ASSERT(attr == NULL) return NULL;
    return attr->pretty_name;
}

const char* get_entity_description(EntityType type)
{
    EntityProperty* attr = get_entity_property(type);
    IF_ASSERT(attr == NULL) return NULL;
    return attr->description;
}

EntityType get_entity_type(const char* name)
{
    EntityProperty* attr = get_entity_property(name);
    IF_ASSERT(attr == NULL) return NULL_ENTITY_TYPE;
    return attr->type;
}

inline bool is_valid_entity_name(const char* name)
{
    return is_valid_name(name);
}

const char* get_compatible_entity_name(const char* name)
{
    const char* mapname = entity_name_map->get_mapped_name(name);
    if (mapname != NULL) return mapname;
    if (get_entity_type(name) != NULL_ENTITY_TYPE) return name;
    return NULL;
}

}   // Entities
