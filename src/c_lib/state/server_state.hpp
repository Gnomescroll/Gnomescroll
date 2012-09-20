#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <agent/agent_list.hpp>
#include <voxel/voxel_hitscan.hpp>

#include <particle/grenade.hpp>

namespace ServerState
{

    extern bool main_inited;
    extern bool signal_exit;
    extern bool should_save_map;

    extern Agent_list* agent_list;
        
    // property lists
    extern Voxel_hitscan_list* voxel_hitscan_list;

    void init();
    void init_lists();
    void teardown();
    void teardown_lists();

    extern class Objects::Object* base;

    struct Vec3 get_base_spawn_position();
    void init_base();
    void check_agents_at_base();

    void damage_objects_within_sphere(
        float x, float y, float z, float radius,
        int damage, int owner,
        ObjectType inflictor_type, int inflictor_id,
        bool suicidal=true
    );

    void spawn_monsters(ObjectType type, int n);
}   // ServerState
