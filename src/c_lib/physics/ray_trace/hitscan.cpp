#include "hitscan.hpp"

#include <t_map/t_map.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <physics/quadrant.hpp>
#include <t_mech/mech_raycast.hpp>
#include <state/state.hpp>

namespace Hitscan
{

// returns agent id of first agent found in hitscan path
// returns NULL_AGENT if none found
AgentID against_agents(Vec3 position, Vec3 direction, float max_distance,
                       AgentID firing_agent_id)
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

#define HITSCAN_PROFILING 0

static WorldHitscanResult _hitscan_against_terrain_and_mobs(const Vec3& p, const Vec3& v,
                                                            float range, int ignore_id,
                                                            EntityType ignore_type)
{
    WorldHitscanResult hitscan;
    hitscan.start_position = p;
    hitscan.direction = v;

    // terrain first
    class RaytraceData terrain_data;
    if (raytrace_terrain(p, v, range, &terrain_data))
    {
        hitscan.set_block_collision(terrain_data, range);
        range = hitscan.block_distance + 1.5f; // cap the range for future calls
    }

    Vec3 voxel_collision_point;
    Voxels::VoxelHitscanTarget voxel_target;
    float voxel_distance;
    if (STATE::voxel_hitscan_list->hitscan(p, v, ignore_id, ignore_type,
                                           range, voxel_collision_point,
                                           voxel_distance, voxel_target))
    {
        range = voxel_distance + 1.5f;
        hitscan.set_voxel_collision(voxel_target, voxel_collision_point,
                                    voxel_distance);
    }

    return hitscan;
}

WorldHitscanResult hitscan_against_terrain_and_mobs(const Vec3& p, const Vec3& v,
                                                    float range, int ignore_id,
                                                    EntityType ignore_type)
{
    WorldHitscanResult hitscan = _hitscan_against_terrain_and_mobs(p, v, range,
                                                                   ignore_id,
                                                                   ignore_type);
    hitscan.finish();
    return hitscan;
}

#if DC_CLIENT
WorldHitscanResult hitscan_against_world(const Vec3& p, const Vec3& v,
                                         float range, int ignore_id,
                                         EntityType ignore_type)
{
    #if HITSCAN_PROFILING
    long ta = _GET_MICROSECOND_TIME();
    #endif

    WorldHitscanResult hitscan = _hitscan_against_terrain_and_mobs(p, v, range,
                                                                   ignore_id,
                                                                   ignore_type);

    #if HITSCAN_PROFILING
    long tb = _GET_MICROSECOND_TIME();
    #endif

    int mech_id;
    float mech_distance;
    if (t_mech::ray_cast_mech(p, v, range, mech_id, mech_distance))
        hitscan.set_mech_collision(mech_id, mech_distance);

    #if HITSCAN_PROFILING
    long tc = _GET_MICROSECOND_TIME();
    printf("\tTerrain and model trace: %0.3fms\n", double(tb-ta)/1000.0);
    printf("\tMech trace:              %0.3fms\n", double(tc-tb)/1000.0);
    printf("Total trace time: %0.3fms\n", double(tc-ta)/1000.0f);
    #endif

    hitscan.finish();
    return hitscan;
}
#endif

}   // Hitscan
