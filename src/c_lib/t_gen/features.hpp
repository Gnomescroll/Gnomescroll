#pragma once

#include <t_map/t_map.hpp>

namespace map_gen
{
    
void ceiling(int x, int y, int z, int height, CubeID tile);
void floor(int x, int y, int z_start, int height, CubeID tile);
void rough_floor(int x, int y, int z_start, int height, CubeID tile);    // white noise
void walls(int x, int y, int z_start, int height, CubeID tile);
void box(int x, int y, int z_start, int height, CubeID tile);

void erosion(const int x, const int y, const int passes, const int h_diff, const int max_iters);

}   // map_gen
