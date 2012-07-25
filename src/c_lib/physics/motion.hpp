#pragma once

#include <physics/vec3.hpp>

int stick_to_terrain_surface(struct Vec3 position);
struct Vec3 move_to_point(struct Vec3 dest, struct Vec3 origin, struct Vec3 momentum);
float orient_to_point(struct Vec3 dest, struct Vec3 origin);
void orient_to_point(struct Vec3 dest, struct Vec3 origin, float* theta, float* phi);

// stick to nearest open block
bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction, float speed, float max_z_diff, struct Vec3* new_position, struct Vec3* new_momentum, int object_height);
bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction, float speed, struct Vec3* new_position, struct Vec3* new_momentum, int object_height);

// stick to highest open block
bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction, float speed, float max_z_diff, struct Vec3* new_position, struct Vec3* new_momentum);
bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction, float speed, struct Vec3* new_position, struct Vec3* new_momentum);
