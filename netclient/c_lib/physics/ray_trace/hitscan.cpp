#include "hitscan.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/physics/ray_trace/ray_trace.hpp>

//forward decl
namespace STATE
{
    extern Voxel_hitscan_list* voxel_hitscan_list;
}

namespace Hitscan {

static HitscanBlock dummy_hitscan_block;

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz)
{
    const float max_dist = 500.0f;  // far
    //const int z_min = 128;
    //const int z_max = 128;
    
    //int *pos = _nearest_block(x, y, z, vx, vy, vz, max_dist, z_min, z_max);
    //if (pos == NULL) {
        //dummy_hitscan_block.hit = false;
        //return &dummy_hitscan_block;
    //}

    float distance=0.0f;
    int collision[3];
    int pre_collision[3];
    int tile=0;
    int side[3];

    int collided = _ray_cast6(x,y,z, vx,vy,vz, max_dist, &distance, collision, pre_collision, &tile, side);

    dummy_hitscan_block.hit = collided;
    if (!collided) return &dummy_hitscan_block;
    
    //dummy_hitscan_block.x = pos[0];
    //dummy_hitscan_block.y = pos[1];
    //dummy_hitscan_block.z = pos[2];
    dummy_hitscan_block.x = collision[0];
    dummy_hitscan_block.y = collision[1];
    dummy_hitscan_block.z = collision[2];
    //dummy_hitscan_block.distance = distance(x,y,z, (float)pos[0], (float)pos[1], (float)pos[2]);
    dummy_hitscan_block.distance = distance;
    dummy_hitscan_block.side[0] = side[0];
    dummy_hitscan_block.side[1] = side[1];
    dummy_hitscan_block.side[2] = side[2];
    dummy_hitscan_block.tile = tile;
    
    return &dummy_hitscan_block;
}

HitscanTargetTypes terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance, int side[3], int *tile)
{

    HitscanBlock* block = ray_intersect_block(x,y,z, vx,vy,vz);

    HitscanTargetTypes target = HITSCAN_TARGET_NONE;

    if (block->hit)
    {
        target = HITSCAN_TARGET_BLOCK;
        pos[0] = block->x;
        pos[1] = block->y;
        pos[2] = block->z;
        *distance = block->distance;
        side[0] = block->side[0];
        side[1] = block->side[1];
        side[2] = block->side[2];
        *tile = block->tile;
    }

    return target;
}


HitscanTargetTypes hitscan_against_world(
    Vec3 p, Vec3 v, int ignore_id, ObjectType ignore_type,    // inputs
    struct Voxel_hitscan_target* target, float* vox_distance, float collision_point[3],
    int block_pos[3], int side[3], int* tile, float* block_distance // outputs
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
