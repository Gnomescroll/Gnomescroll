#pragma once

#include <physics/vec3i.hpp>

// deprecate in favor of Vec3i map_dim
const int XMAX = 512;
const int YMAX = XMAX;
const int ZMAX = 128;

struct Vec3i map_dim = vec3i_init(XMAX, YMAX, ZMAX);    // not really constant..

const int MAX_CUBES = 0xFF;

const int TERRAIN_MAP_HEIGHT = 128;
const int TERRAIN_CHUNK_WIDTH = 16; // width of a chunk

const int TERRAIN_MAP_HEIGHT_BIT_MASK = ~(TERRAIN_MAP_HEIGHT - 1);
const int TERRAIN_MAP_WIDTH_BIT_MASK = ~(XMAX - 1); //assumes map size of 512
const int TERRAIN_MAP_WIDTH_BIT_MASK2 = (XMAX - 1); //assumes map size of 512

const int CHUNK_MAP_WIDTH_BIT_MASK = ~(XMAX/TERRAIN_CHUNK_WIDTH - 1); //assumes map size of 512
const int CHUNK_MAP_WIDTH_BIT_MASK2 = (XMAX/TERRAIN_CHUNK_WIDTH - 1); //assumes map size of 512

const int MAP_CHUNK_XDIM = XMAX/TERRAIN_CHUNK_WIDTH;   // # of chunks in x
const int MAP_CHUNK_YDIM = XMAX/TERRAIN_CHUNK_WIDTH;  // # of chunks in y

const float QUADRANT_DIVIDEf = float(XMAX) / 2.0f;
const int QUADRANT_DIVIDEi = XMAX / 2;

const unsigned char MAX_CUBE_DAMAGE = 0XFF;
const unsigned char INVINCIBLE_CUBE_DAMAGE = MAX_CUBE_DAMAGE;   // any blocks with this value will be invincible

const size_t CHUNK_ELEMENT_COUNT = ZMAX*TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH;
