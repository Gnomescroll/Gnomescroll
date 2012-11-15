#include "hitscan.hpp"

#include <t_map/t_map.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <physics/quadrant.hpp>

//forward decl
namespace STATE
{
    extern Voxel_hitscan_list* voxel_hitscan_list;
}

namespace Hitscan
{

static HitscanBlock dummy_hitscan_block;

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz)
{
    const float MAX_DIST = 0.95f * (float)t_map::map_dim.z;  // far

    //float distance=0.0f;
    //int collision[3];
    //int pre_collision[3];
    //CubeID tile = EMPTY_CUBE;
    //int side[3];

    //int collided = _ray_cast6(x,y,z, vx,vy,vz, max_dist, &distance, collision, pre_collision, &tile, side);

    struct RaytraceData data;
    struct Vec3 start = vec3_init(x, y, z);
    struct Vec3 direction = vec3_init(vx, vy, vz);
    struct Vec3 end = vec3_add(start, vec3_scalar_mult(direction, MAX_DIST));
    bool collided = raytrace_terrain(start, end, &data);

    dummy_hitscan_block.hit = collided;
    if (!collided) return &dummy_hitscan_block;
    
    dummy_hitscan_block.x = data.collision_point[0];
    dummy_hitscan_block.y = data.collision_point[1];
    dummy_hitscan_block.z = data.collision_point[2];

    dummy_hitscan_block.distance = MAX_DIST * data.interval;
    data.get_side_array(dummy_hitscan_block.side);
    dummy_hitscan_block.tile = t_map::get(dummy_hitscan_block.x, dummy_hitscan_block.y, dummy_hitscan_block.z);
    
    return &dummy_hitscan_block;
}

HitscanTargetTypes terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance, int side[3], CubeID *tile)
{
    HitscanBlock* block = ray_intersect_block(x,y,z, vx,vy,vz);

    HitscanTargetTypes target = HITSCAN_TARGET_NONE;

    if (block->hit)
    {
        target = HITSCAN_TARGET_BLOCK;
        pos[0] = translate_point(block->x);
        pos[1] = translate_point(block->y);
        pos[2] = block->z;
        *distance = block->distance;
        side[0] = block->side[0];
        side[1] = block->side[1];
        side[2] = block->side[2];
        *tile = block->tile;
    }
    else
        *tile = EMPTY_CUBE;

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
    int block_pos[3], int side[3], CubeID* tile, float* block_distance // outputs
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
        side, tile
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
