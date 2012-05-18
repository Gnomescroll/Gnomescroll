#pragma once

#include <t_map/t_map.hpp>

#include <map_gen/perlin.h>
#include <map_gen/noise.hpp>

namespace map_gen
{
    
void grass(int x, int y, int base, int dirt, int grass);
void caves(int x, int y, int z, float threshold, int base);
void ceiling(int x, int y, int z, int height, int tile);
void floor(int x, int y, int z_start, int height, int tile);
void walls(int x, int y, int z_start, int height, int tile);
void box(int x, int y, int z_start, int height, int tile);

}   // map_gen
