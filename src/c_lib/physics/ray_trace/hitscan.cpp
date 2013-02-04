#include "hitscan.hpp"

#include <t_map/t_map.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <physics/quadrant.hpp>
#if DC_SERVER
# include <state/server_state.hpp>
#endif
#if DC_CLIENT
# include <state/client_state.hpp>
#endif

namespace Hitscan
{

// returns agent id of first agent found in hitscan path
// returns NULL_AGENT if none found
AgentID against_agents(Vec3 position, Vec3 direction, float max_distance, AgentID firing_agent_id)
{
    float vox_distance = 1000000.0f;
    class Voxels::VoxelHitscanTarget target;
    float collision_point[3];

    // TODO -- keep agents in their own hitscan list
    bool hit = STATE::voxel_hitscan_list->hitscan(position, direction,
                                                  firing_agent_id, OBJECT_AGENT,
                                                  collision_point, &vox_distance,
                                                  &target);
    if (!hit) return NULL_AGENT;
    if (target.entity_type != OBJECT_AGENT) return NULL_AGENT;
    if (vox_distance > max_distance) return NULL_AGENT;
    return (AgentID)target.entity_id;
}

AgentID against_agents(Vec3 position, Vec3 direction, float max_distance)
{
    return against_agents(position, direction, max_distance, NULL_AGENT);
}

HitscanTargetTypes hitscan_against_world(
    struct Vec3 p, struct Vec3 v, int ignore_id, EntityType ignore_type,        // inputs
    class Voxels::VoxelHitscanTarget* target, float* vox_distance, float collision_point[3],
    int block_pos[3], int side[3], CubeType* cube_type, float* block_distance)  // outputs
{
    *vox_distance = 10000000.0f;
    bool voxel_hit = STATE::voxel_hitscan_list->hitscan(p, v, ignore_id, ignore_type,
                                                        collision_point, vox_distance,
                                                        target);
    class RaytraceData terrain_data;
    const float max_dist = 128.0f;
    bool block_hit = raytrace_terrain(p, v, max_dist, &terrain_data);
    if (block_hit)
    {
        *block_distance = terrain_data.interval * max_dist;
        *cube_type = terrain_data.get_cube_type();
        terrain_data.get_side_array(side);
        for (int i=0; i<3; i++) block_pos[i] = terrain_data.collision_point[i];
    }
    else
    {
        *block_distance = 10000000.0f;
        *cube_type = NULL_CUBE;
        side[2] = 1;
        for (int i=0; i<3; i++) block_pos[i] = -1;
    }

    // TODO -- add mech hitscan

    // choose closer collision (or none)
    HitscanTargetTypes target_type = HITSCAN_TARGET_NONE;
    bool voxel_closer = (*vox_distance <= *block_distance);
    if (voxel_hit && block_hit)
    {
        if (voxel_closer)
            target_type = HITSCAN_TARGET_VOXEL;
        else
            target_type = HITSCAN_TARGET_BLOCK;
    }
    else if (voxel_hit)
        target_type = HITSCAN_TARGET_VOXEL;
    else if (block_hit)
        target_type = HITSCAN_TARGET_BLOCK;
    else
        target_type = HITSCAN_TARGET_NONE;

    return target_type;
}

}   // Hitscan
