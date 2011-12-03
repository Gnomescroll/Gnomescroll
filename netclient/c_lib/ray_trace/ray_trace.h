#pragma once

#include <stdio.h>
#include <math.h>

#include <compat.h>


#define ssize 256
#define bsize 65536

#define RAYCAST_SAMPLING_DENSITY 100.0f;

#define raycast_tick_max 1024

//int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1);
//int ray_cast_lax(float x0,float y0,float z0, float x1,float y1,float z1);

//int* _ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1, float* distance);


//used by grenade
int* _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval);

int* _ray_cast5(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, int* tile);
int* _ray_cast5_capped(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, int* tile);
int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, int* tile, int* side);

// args: position and forward vector
int* _nearest_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high);
int* _farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high);

float sphere_line_distance(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz, float* pos, float* _rad2);
