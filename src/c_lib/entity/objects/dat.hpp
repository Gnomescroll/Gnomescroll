#pragma once

namespace Entities
{

#define ENTITY_NAME_FILE_ACTIVE   "entity_names.active"
#define ENTITY_NAME_FILE_INACTIVE "entity_names.inactive"

// Attribute class

class EntityAttributes
{
    public:
        bool loaded;
        
        EntityType type;
        char name[DAT_NAME_MAX_LENGTH+1];

        //// Controllers
        //tickObject* tick;
        //updateObject* update;

        //// Network
        //class CreatePacketDelegate* create;
        //class StatePacketDelegate* state;

        //// Component Properties: 

        //// dimension
        //float dim_height;
        //float dim_camera_height;

        //// voxel model
        //class VoxDat* vox_dat;
        //bool vox_init_hitscan;
        //bool vox_init_draw;

        //// health
        //int initial_health;
        //int max_health;

        //// agent spawner
        //float agent_spawn_radius;

        //// monster spawner
        //float monster_spawn_radius;
        //int monster_spawn_max_children;
        //EntityType monster_spawn_type;

        //// healer
        //float heal_radius;

        //// ITEM DROP WILL BE EXTERNAL
        ////int item_drop_id;

        //// rate limit
        //int rate_limit;

        //// animations
        //int anim_count;
        //int anim_count_max;
        //float anim_size;
        //float anim_force;
        //struct Color anim_color;

        //// motion targeting (( DEPRECATE ?? [for Dest targeting]))
        //float motion_speed;
        //int motion_max_z_diff;

        //// weapon targeting
        //float weapon_acquisition_rate;
        //int weapon_fire_rate;
        //bool weapon_uses_bias;
        //bool weapon_accuracy_bias;
        //float weapon_sight_range;
        //bool weapon_attack_at_random;
        //int weapon_terrain_damage;
        //int weapon_agent_damage_min;
        //int weapon_agent_damage_max;
        //int weapon_fire_delay;
        //int weapon_agent_protection_duration;
        //TerrainModificationAction weapon_tma;

        //// destination targeting
        //float dest_sight_range;
        //float dest_choice_x;
        //float dest_choice_y;
        //float dest_speed;
        //int dest_max_z_diff;

        //// agent targeting
        //float agent_sight_range;
        //float agent_speed;
        //int agent_max_z_diff;
        //int agent_max_lock_ticks;

        //// state machine
        //EntityState state_initial;
        //stateRouter* state_router;

        //// waiting
        //int wait_time;

        //// explosion
        //float explosion_radius;
        //float explosion_proximity_radius;
        //float explosion_block_destruction_radius;
        //int explosion_damage;
        //int explosion_block_damage;
        //bool explosion_harms_owner;
        //TerrainModificationAction explosion_tma;


        // aggregate all properties a component might want to load from
        // set_blah_properties will automatically look up from the dat

        // every component will need an unpacker

        // drops will have to be configured separately as usual

    EntityAttributes() : loaded(false),
        type(OBJECT_NONE)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};

extern class EntityAttributes* attributes;
extern class DatNameMap* entity_name_map;

void init_entity_dat();
void teardown_entity_dat();

// Validate
inline bool is_valid_entity_name(const char* name)
{
    return is_valid_name(name);
}

void verify_entity_dat();
void end_entity_dat();

// Use this to remove or rename a entity
void change_entity_name(const char* original, const char* replacement);

void entity_def(const char* name, EntityType type);
void finish_def();

// Load
void load_entity_dat()
{
    entity_def("base", OBJECT_BASE);
    entity_def("agent_spawner", OBJECT_AGENT_SPAWNER);
    entity_def("turret", OBJECT_TURRET);
    entity_def("energy_core", OBJECT_ENERGY_CORE);
    entity_def("blue_bomb", OBJECT_MONSTER_BOMB);
    entity_def("red_fish", OBJECT_MONSTER_BOX);
    entity_def("monster_spawner", OBJECT_MONSTER_SPAWNER);

    finish_def();
}

void apply_entity_dat_changes()
{
    //change_entity("old_name", "new_name");
    entity_name_map->condense();  // finalize
}


EntityType get_entity_type(const char* name)
{   // TODO -- use hash map
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
        if (attributes[i].loaded && strcmp(name, attributes[i].name) == 0)
            return (EntityType)i;
    return OBJECT_NONE;
}

const char* get_compatible_entity_name(const char* name)
{
    const char* mapname = entity_name_map->get_mapped_name(name);
    if (mapname != NULL) return mapname;
    if (get_entity_type(name) != OBJECT_NONE) return name;
    return NULL;
}

const class EntityAttributes* get_entity_attributes(EntityType type)
{
    ASSERT_VALID_OBJECT_TYPE(type);
    IF_INVALID_OBJECT_TYPE(type) return NULL;
    GS_ASSERT(attributes[type].loaded);
    if (!attributes[type].loaded) return NULL;
    return &attributes[type];
}

const char* get_entity_name(EntityType type)
{
    const class EntityAttributes* attr = get_entity_attributes(type);
    if (attr == NULL) return NULL;
    return attr->name;
}

}   // Entities
