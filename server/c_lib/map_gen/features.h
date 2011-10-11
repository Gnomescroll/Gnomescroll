#pragma once

#include <c_lib/t_map/t_map.h>

#include "perlin.h"
#include "noise.h"

void _grass(int x, int y, int base);
void _caves(int x, int y, int z, float threshold, int base);
