#include "hitscan.hpp"

#include <t_map/t_map.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <physics/quadrant.hpp>

#if DC_SERVER
#include <state/server_state.hpp>
#endif

#if DC_CLIENT
#include <state/client_state.hpp>
#endif

namespace Hitscan
{

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz)
{
    static HitscanBlock dummy;

    const float MAX_DIST = 128.0f;

    struct RaytraceData data;
    struct Vec3 start = vec3_init(x, y, z);
    struct Vec3 direction = vec3_init(vx, vy, vz);
    struct Vec3 end = vec3_add(start, vec3_scalar_mult(direction, MAX_DIST));
    bool collided = raytrace_terrain(start, end, &data);

    if (!collided) return NULL;
    
    dummy.x = data.collision_point[0];
    dummy.y = data.collision_point[1];
    dummy.z = data.collision_point[2];

    dummy.distance = MAX_DIST * data.interval;

    //printf("Interval: %0.2f, Distance: %0.2f\n", data.interval, dummy.distance);
    //printf("Est. distance: %0.2f\n", vec3_distance(vec3_init(x,y,z), vec3_init(dummy.x, dummy.y, dummy.z)));
    
    data.get_side_array(dummy.side);
    dummy.cube_id = t_map::get(dummy.x, dummy.y, dummy.z);
    
    return &dummy;
}

HitscanTargetTypes terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance, int side[3], CubeID *cube_id)
{
    HitscanBlock* block = ray_intersect_block(x,y,z, vx,vy,vz);

    HitscanTargetTypes target = HITSCAN_TARGET_NONE;

    *cube_id = EMPTY_CUBE;
    if (block != NULL)
    {
        target = HITSCAN_TARGET_BLOCK;
        pos[0] = translate_point(block->x);
        pos[1] = translate_point(block->y);
        pos[2] = block->z;
        *distance = block->distance;
        for (int i=0; i<3; i++) side[i] = block->side[i];
        *cube_id = block->cube_id;
    }

    return target;
}

// returns agent id of first agent found in hitscan path
// returns NULL_AGENT if none found
AgentID against_agents(Vec3 position, Vec3 direction, float max_distance, AgentID firing_agent_id)
{
    float vox_distance = 1000000.0f;
    class Voxel_hitscan_target target;
    float collision_point[3];
    
    // TODO -- keep agents in their own hitscan list
    bool hit = STATE::voxel_hitscan_list->hitscan(
        position.x, position.y, position.z,
        direction.x, direction.y, direction.z,
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
    Vec3 p, Vec3 v, int ignore_id, EntityType ignore_type,    // inputs
    class Voxel_hitscan_target* target, float* vox_distance, float collision_point[3],
    int block_pos[3], int side[3], CubeID* cube_id, float* block_distance // outputs
)
{   // hitscan against voxels
    *vox_distance = 10000000.0f;
    bool voxel_hit = STATE::voxel_hitscan_list->hitscan(
        p.x, p.y, p.z,
        v.x, v.y, v.z,
        ignore_id, ignore_type,
        collision_point, vox_distance,
        target
    );

     //hitscan against terrain
    *block_distance = 10000000.0f;
    HitscanTargetTypes target_type = terrain(
        p.x, p.y, p.z,
        v.x, v.y, v.z,
        block_pos, block_distance,
        side, cube_id
    );

    // choose closer collision (or none)
    bool block_hit = (target_type == HITSCAN_TARGET_BLOCK);
    bool voxel_closer = (*vox_distance <= *block_distance);

    //printf("voxel closer = %d\n", voxel_closer);
    //printf("vox distance = %0.2f\n", *vox_distance);

    if (voxel_hit && block_hit)
        if (voxel_closer)
            target_type = HITSCAN_TARGET_VOXEL;
        else
            target_type = HITSCAN_TARGET_BLOCK;
    else if (voxel_hit)
        target_type = HITSCAN_TARGET_VOXEL;
    else if (block_hit)
        target_type = HITSCAN_TARGET_BLOCK;
    else
        target_type = HITSCAN_TARGET_NONE;

    return target_type;
}

}
