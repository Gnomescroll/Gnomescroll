#pragma once

namespace Hitscan {
    
enum {
    HITSCAN_TARGET_NONE,
    HITSCAN_TARGET_BLOCK
};

struct HitscanBlock {
    int hit;
    int x,y,z;
    float distance;
    int side[3];
    int tile;
};

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz);

//int terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance);
int terrain(float x, float y, float z, float vx, float vy, float vz, int pos[3], float *distance, int side[3], int *tile);
}
