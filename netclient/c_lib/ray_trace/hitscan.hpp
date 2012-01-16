#pragma once

namespace Hitscan {
    
enum {
    HITSCAN_TARGET_NONE,
    HITSCAN_TARGET_BLOCK
};

struct HitscanBlock {
    bool hit;
    int x,y,z;
    float distance;
};

//HitscanAgent* ray_intersect_agent(float x, float y, float z, float vx, float vy, float vz);
HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz);

int terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance);

}
