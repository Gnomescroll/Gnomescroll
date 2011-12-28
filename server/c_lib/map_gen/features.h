#pragma once

#include <c_lib/t_map/t_map.hpp>

#include "perlin.h"
#include "noise.h"

void _grass(int x, int y, int base);
void _caves(int x, int y, int z, float threshold, int base);
void _ceiling(int x, int y, int z, int height, int tile);
void _floor(int x, int y, int z_start, int height, int tile);
