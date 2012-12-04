#pragma once

#include <physics/vec3.hpp>
#include <voxel/constants.hpp>

#if DC_CLIENT

static float v_buffer[3*8];
static float s_buffer[6*(4*3)];

static const struct Vec3 base_normals[6] = {
    {{{1,0,0}}},
    {{{0,1,0}}},
    {{{0,0,1}}},
    {{{-1,0,0}}},
    {{{0,-1,0}}},
    {{{0,0,-1}}},
};

#endif
