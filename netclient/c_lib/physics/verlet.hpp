#pragma once

#include <c_lib/physics/vec3.hpp>

namespace Verlet
{

bool bounce(Vec3* old_position, Vec3* old_velocity, Vec3* position, Vec3* velocity, float damp);
int* bounce(Vec3* old_position, Vec3* old_velocity, Vec3* position, Vec3* velocity, float damp, int* collision, int* tile);

}

