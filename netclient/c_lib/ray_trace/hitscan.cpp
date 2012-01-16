#include "hitscan.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/ray_trace/ray_trace.h>
#include <c_lib/state/client_state.hpp>


namespace Hitscan {

//static HitscanAgent dummy_hitscan_agent;
static HitscanBlock dummy_hitscan_block;

//HitscanAgent* ray_intersect_agent(float x, float y, float z, float vx, float vy, float vz, int ignore_agent) {

    //float pos[3];
    //float _rad2=0.0f,*__rad2=&_rad2;
    //float _dist=0.0f,*dist=&_dist;
    //Agent_state* a = STATE::agent_list.hitscan_agents(x,y,z, vx,vy,vz, pos, __rad2, dist, ignore_agent);
    //if (a == NULL) {
        //dummy_hitscan_agent.hit = false;
        //return &dummy_hitscan_agent;
    //}
    //dummy_hitscan_agent.hit = true;
    //dummy_hitscan_agent.x = a->s.x;
    //dummy_hitscan_agent.y = a->s.y;
    //dummy_hitscan_agent.z = a->s.z;
    //dummy_hitscan_agent.distance = *dist;
    //dummy_hitscan_agent.id = a->id;
    ////dummy_hitscan_agent.body_part = body_part;
    //dummy_hitscan_agent.body_part = 0;
    
    //return &dummy_hitscan_agent;
//}

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz) {
    const float max_dist = 500.0f;  // far
    const int z_min = 128;
    const int z_max = 128;
    
    int *pos = _nearest_block(x, y, z, vx, vy, vz, max_dist, z_min, z_max);
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

int terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance) {

    HitscanBlock* block = ray_intersect_block(x,y,z, vx,vy,vz);

    int target = HITSCAN_TARGET_NONE;

    if (block->hit) {
        target = HITSCAN_TARGET_BLOCK;
        pos[0] = block->x;
        pos[1] = block->y;
        pos[2] = block->z;
        *distance = block->distance;
    }

    return target;
}

}
