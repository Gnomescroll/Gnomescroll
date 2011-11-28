#include "hitscan.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/ray_trace/ray_trace.h>

namespace Hitscan {

static HitscanAgent dummy_hitscan_agent;
static HitscanBlock dummy_hitscan_block;

HitscanAgent* ray_intersect_agent(float x, float y, float z, float vx, float vy, float vz) {
    dummy_hitscan_agent.hit = false;
    return &dummy_hitscan_agent;
}

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz) {
    const float max_dist = 500.0f;  // far
    int *pos = _nearest_block(x, y, z, vx, vy, vz, max_dist, 0, zmax);
    if (pos == NULL) {
        dummy_hitscan_block.hit = false;
        return &dummy_hitscan_block;
    }
    dummy_hitscan_block.hit = true;
    dummy_hitscan_block.x = pos[0];
    dummy_hitscan_block.y = pos[1];
    dummy_hitscan_block.z = pos[2];
    dummy_hitscan_block.distance = distance(x,y,z, (float)pos[0], (float)pos[1], (float)pos[2]);
    
    return &dummy_hitscan_block;
}

int resolve_hitscan_target(float x, float y, float z, float vx, float vy, float vz, int data[3]) {
    // data will be filled with metadata for the target
    // function returns hitscan target type

    HitscanAgent* agent = ray_intersect_agent(x,y,z, vx,vy,vz);
    HitscanBlock* block = ray_intersect_block(x,y,z, vx,vy,vz);

    int target = HITSCAN_TARGET_NONE;

    if (agent->hit && block->hit) {
        if (agent->distance < block->distance) {
            target = HITSCAN_TARGET_AGENT;
        } else {
            target = HITSCAN_TARGET_BLOCK;
        }
    } else if (agent->hit) {
        target = HITSCAN_TARGET_AGENT;
    } else if (block->hit) {
        target = HITSCAN_TARGET_BLOCK;
    }

    switch (target) {
        case HITSCAN_TARGET_AGENT:
            data[0] = agent->id;
            data[1] = agent->body_part;
            break;

        case HITSCAN_TARGET_BLOCK:
            data[0] = block->x;
            data[1] = block->y;
            data[2] = block->z;
            break;
    }
        
    return target;
}

}
