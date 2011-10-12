#pragma once

#include "perlin.h"

void perturb_perlin2(int x, int y, int z, float turbulence);
void perturb_perlin3(int x, int y, int z, float turbulence);
void perturb_heightmap(int x, int y, float turbulence, int tile, int clamp);

void perturb_2d_noisemap(int x, int y, float turbulence, int blend_mode);

enum NOISE_BLEND {
    NO_BLEND,   // a = b
    ADD_BLEND,  // a += b
    SUB_BLEND,  // a -= b
    MULT_BLEND, // a *= b
    DIV_BLEND   // a /= b
};
