#pragma once

#include <c_lib/physics/vec3.hpp>

namespace Verlet
{

bool bounce(Vec3* position, Vec3* velocity, float damp);
int* bounce(Vec3* position, Vec3* velocity, float damp, int* collision, int* tile);

}

