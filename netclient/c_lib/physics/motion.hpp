#pragma once

// forward decl
struct Vec3;

int stick_to_terrain_surface(Vec3 position);
Vec3 move_to_point(Vec3 dest, Vec3 origin, Vec3 momentum);
float orient_to_point(Vec3 dest, Vec3 origin);
void orient_to_point(Vec3 dest, Vec3 origin, float* theta, float* phi);

bool move_along_terrain_surface(Vec3 position, Vec3 direction, float speed, Vec3* new_position, Vec3* new_momentum);
bool move_along_terrain_surface(Vec3 position, Vec3 direction, float speed, float max_z_down, float max_z_up, Vec3* new_position, Vec3* new_momentum);
