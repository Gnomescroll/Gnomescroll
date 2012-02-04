#include "hitscan.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/ray_trace/ray_trace.h>
#include <c_lib/state/client_state.hpp>


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

int terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance, int side[3], int *tile)
{

    HitscanBlock* block = ray_intersect_block(x,y,z, vx,vy,vz);

    int target = HITSCAN_TARGET_NONE;

    if (block->hit) {
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

}
