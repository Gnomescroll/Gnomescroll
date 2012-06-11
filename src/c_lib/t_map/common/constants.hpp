#pragma once

// deprecate in favor of MapDimension map_dim
const int XMAX = 512;
const int YMAX = 512;
const int ZMAX = 128;

namespace t_map
{

const int TERRAIN_MAP_HEIGHT = 128;
const int TERRAIN_CHUNK_WIDTH = 16;

const int TERRAIN_MAP_HEIGHT_BIT_MASK = ~(TERRAIN_MAP_HEIGHT-1);
const int TERRAIN_MAP_WIDTH_BIT_MASK = ~(512-1); //assumes map size of 512
const int TERRAIN_MAP_WIDTH_BIT_MASK2 = (512-1); //assumes map size of 512

const int CHUNK_MAP_WIDTH_BIT_MASK = ~(512/16 -1); //assumes map size of 512
const int CHUNK_MAP_WIDTH_BIT_MASK2 = (512/16- 1); //assumes map size of 512

const int MAP_WIDTH = 512;
const int MAP_HEIGHT = 512;

const int MAP_CHUNK_WIDTH = MAP_WIDTH/TERRAIN_CHUNK_WIDTH;
const int MAP_CHUNK_HEIGHT = MAP_HEIGHT/TERRAIN_CHUNK_WIDTH;

const int MAP_CHUNK_XDIM = MAP_WIDTH/TERRAIN_CHUNK_WIDTH;
const int MAP_CHUNK_YDIM = MAP_HEIGHT/TERRAIN_CHUNK_WIDTH;

} // t_map

const float QUADRANT_DIVIDEf = 256.0f;
const int QUADRANT_DIVIDEi = 256;
