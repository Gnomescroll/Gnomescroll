#pragma once

namespace Hitscan {
    
enum {
    HITSCAN_TARGET_NONE,
    HITSCAN_TARGET_AGENT,
    HITSCAN_TARGET_BLOCK
};

struct HitscanAgent {
    bool hit;
    float x,y,z;
    int id;
    int body_part;
    float distance;
};

struct HitscanBlock {
    bool hit;
    int x,y,z;
    float distance;
};

HitscanAgent* ray_intersect_agent(float x, float y, float z, float vx, float vy, float vz);
HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz);

int resolve_hitscan_target(float x, float y, float z, float vx, float vy, float vz, int data[3]);

}
