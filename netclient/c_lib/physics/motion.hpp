#pragma once

#include <c_lib/physics/vec3.hpp>

int stick_to_terrain_surface(Vec3 position);
Vec3 move_to_point(Vec3 dest, Vec3 origin, Vec3 momentum);
float orient_to_point(Vec3 dest, Vec3 origin);
void orient_to_point(Vec3 dest, Vec3 origin, float* theta, float* phi);
