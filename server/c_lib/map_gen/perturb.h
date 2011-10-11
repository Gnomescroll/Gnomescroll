#pragma once

#include "perlin.h"

void perturb_perlin2(int x, int y, int z, float turbulence);
void perturb_perlin3(int x, int y, int z, float turbulence);
void perturb_heightmap(int x, int y, float turbulence, int tile, int clamp);
