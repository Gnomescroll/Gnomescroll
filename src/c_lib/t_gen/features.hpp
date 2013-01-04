#pragma once

#include <t_map/t_map.hpp>

namespace map_gen
{
    
void ceiling(int x, int y, int z, int height, CubeType tile);
void floor(int x, int y, int z_start, int height, CubeType tile);
void rough_floor(int x, int y, int z_start, int height, CubeType tile);    // white noise
void walls(int x, int y, int z_start, int height, CubeType tile);
void box(int x, int y, int z_start, int height, CubeType tile);

void fill(const char* cube_name);

void erosion(const int x, const int y, const int passes, const int h_diff, const int max_iters);

}   // map_gen
