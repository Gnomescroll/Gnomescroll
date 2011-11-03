#pragma once

#ifdef DC_CLIENT
// vertex buffers for voxels
const int v_set[3*8] = {
        0,0,0,
        1,0,0,
        1,1,0,
        0,1,0,
        0,0,1,
        1,0,1,
        1,1,1,
        0,1,1 };

const int q_set[4*6]= {
        4,5,6,7,
        3,2,1,0,
        2,3,7,6,
        0,1,5,4,
        0,4,7,3,
        1,2,6,5 };

float v_buffer[3*8];
float s_buffer[6*(4*3)];


struct Voxel {
unsigned char r,g,b,a;
};

#endif
