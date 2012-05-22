#pragma once

#include <stdlib.h>
#include <limits.h>

int seed_max = INT_MAX;

#include <t_map/t_map.hpp>

typedef union f3
{
    float xyz[3];
    float x, y, z;
} float3;

typedef union f4
{
    float xyzw[4];
    float x, y, z, w;
} float4;

const float GRAD3[][3] =
{
    {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0}, 
    {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1}, 
    {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},
    {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}
;

#define fastfloor(n) (int)(n) - (((n) < 0.0f) & ((n) != (int)(n)))

const int PERM_SIZE = 256;
// At the possible cost of unaligned access, we use char instead of
// int here to try to ensure that this table fits in L1 cache
extern unsigned char* PERM;

void noise_init();
void noise_teardown();

extern int _oct;
extern float _per;
extern float _amp;
extern float _lac;
extern float _freq;

extern float xnoise_scale;
extern float ynoise_scale;
extern float znoise_scale;

extern int _base_seed;
extern int _seed;

int seed_noise(int seed);
int next_seed();
int set_seed_grp(int grp);

void set_heightmap_tile(int tile);
void set_terrain_density(float* noisemap, int x, int y, int z, float threshold, int tile);
void invert_map(int x, int y, int z, int tile);
void set_terrain_height(float* noisemap, int x, int y, int z, int baseline, int maxheight, int tile);
void reverse_heightmap (float* noisemap, int x, int y, int z, int baseline, int maxheight, int minheight, int tile);
void set_terrain_height_over_tile(float* noisemap, int x, int y, int z, int baseline, int maxheight, int tile);
void reverse_heightmap_over_tile (float* noisemap, int x, int y, int z, int baseline, int maxheight, int minheight, int tile);

void set_noise_parameters(int octaves, float persistence, float amplitude, float lacunarity, float frequency);
void set_noise_scale(float xscale, float yscale, float zscale);

float* create_noisemap(int x, int y, int z);
void destroy_noisemap();
void clear_noisemap(float* noisemap);
