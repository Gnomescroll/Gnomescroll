#include "hitscan.hpp"

#include <t_map/t_map.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <physics/quadrant.hpp>
#include <t_mech/mech_raycast.hpp>
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
    Vec3 collision_point;

    // TODO -- keep agents in their own hitscan list
    bool hit = STATE::voxel_hitscan_list->hitscan(position, direction,
                                                  firing_agent_id, OBJECT_AGENT,
                                                  collision_point, vox_distance,
                                                  target);
    if (!hit) return NULL_AGENT;
    if (target.entity_type != OBJECT_AGENT) return NULL_AGENT;
    if (vox_distance > max_distance) return NULL_AGENT;
    return (AgentID)target.entity_id;
}

AgentID against_agents(Vec3 position, Vec3 direction, float max_distance)
{
    return against_agents(position, direction, max_distance, NULL_AGENT);
}

WorldHitscanResult hitscan_against_world(const Vec3& p, const Vec3& v,
                                         float range, int ignore_id,
                                         EntityType ignore_type)
{
    WorldHitscanResult hitscan;

    // terrain first
    class RaytraceData terrain_data;
    if (raytrace_terrain(p, v, range, &terrain_data))
    {
        hitscan.set_block_collision(terrain_data, range);
        range = hitscan.block_distance; // cap the range for future calls
    }

    Vec3 voxel_collision_point;
    Voxels::VoxelHitscanTarget voxel_target;
    float voxel_distance;
    if (STATE::voxel_hitscan_list->hitscan(p, v, ignore_id, ignore_type,
                                           range, voxel_collision_point,
                                           voxel_distance, voxel_target))
    {
        range = voxel_distance;
        hitscan.set_voxel_collision(voxel_target, voxel_collision_point,
                                    voxel_distance);
    }

    int mech_id;
    float mech_distance;
    if (t_mech::ray_cast_mech(p, v, range, mech_id, mech_distance))
        hitscan.set_mech_collision(mech_id, mech_distance);

    hitscan.finish();
    return hitscan;
}

}   // Hitscan
