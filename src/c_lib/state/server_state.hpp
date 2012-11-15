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
    extern bool reload_settings;

    extern class Entities::Entity* base;

    // property lists
    extern Voxel_hitscan_list* voxel_hitscan_list;

    void init_lists();
    void teardown_voxel_lists();

    struct Vec3 get_base_spawn_position();
    void init_base();
    void check_agents_at_base();

    void damage_objects_within_sphere(
        struct Vec3 position, float radius,
        int damage, AgentID owner,
        EntityType inflictor_type, int inflictor_id,
        bool suicidal=true
    );

    void spawn_monsters(EntityType type, int n);
}   // ServerState
