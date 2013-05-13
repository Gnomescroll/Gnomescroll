#pragma once

#include <physics/vec3.hpp>

int stick_to_terrain_surface(struct Vec3 position);
struct Vec3 move_to_point(struct Vec3 dest, struct Vec3 origin, struct Vec3 momentum);
float orient_to_point(struct Vec3 dest, struct Vec3 origin);
void orient_to_point(struct Vec3 dest, struct Vec3 origin, float* theta, float* phi);

// These stick to the highest open block;
// The ones that accept max_z_diff will not move if the difference in z level required to move is greater
// The ones that accept object_height will use object_height to determine if the destination has enough vertical space to fit the object
void move_along_terrain_surface(
    struct Vec3 position, struct Vec3 direction, float speed,
    float max_z_diff, struct Vec3* new_position, struct Vec3* new_momentum,
    int object_height);
void move_along_terrain_surface(
    struct Vec3 position, struct Vec3 direction, float speed,
    float max_z_diff, struct Vec3* new_position, struct Vec3* new_momentum);
void move_along_terrain_surface(
    struct Vec3 position, struct Vec3 direction, float speed,
    struct Vec3* new_position, struct Vec3* new_momentum, int object_height);
void move_along_terrain_surface(
    struct Vec3 position, struct Vec3 direction, float speed,
    struct Vec3* new_position, struct Vec3* new_momentum);

// These will move underneath blocks if it makes sense to
void move_within_terrain_surface(
    struct Vec3 position, struct Vec3 direction, float speed,
    float max_z_diff, struct Vec3* new_position, struct Vec3* new_momentum);
void move_within_terrain_surface(
    struct Vec3 position, struct Vec3 direction, float speed,
    float max_z_diff, struct Vec3* new_position, struct Vec3* new_momentum,
    int object_height);
