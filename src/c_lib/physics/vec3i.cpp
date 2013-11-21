/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "vec3i.hpp"

#include <math.h>
#include <physics/quadrant.hpp>

float euclidean_distance_2d(const struct Vec3i& a, const struct Vec3i& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    return sqrtf(x*x + y*y);
}

int diagonal_distance_2d(const struct Vec3i& a, const struct Vec3i& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    return GS_MAX(abs(x), abs(y));
}

int manhattan_distance_2d(const struct Vec3i& a, const struct Vec3i& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    return (abs(x) + abs(y));
}

float euclidean_distance(const struct Vec3i& a, const struct Vec3i& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    int z = quadrant_translate(b.z, a.z) - b.z;
    return sqrtf(x*x + y*y + z*z);
}

int diagonal_distance(const struct Vec3i& a, const struct Vec3i& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    int z = quadrant_translate(b.z, a.z) - b.z;
    return GS_MAX(abs(z), GS_MAX(abs(x), abs(y)));
}

int manhattan_distance(const struct Vec3i& a, const struct Vec3i& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    int z = quadrant_translate(b.z, a.z) - b.z;
    return (abs(x) + abs(y) + abs(z));
}

inline struct Vec3i vec3i_add(struct Vec3i pos, const struct Vec3i& off)
{   // NOTE: returns the vec3i translated
    pos.x = pos.x + off.x;
    pos.y = pos.y + off.y;
    pos.z = pos.z + off.z;
    return translate_position(pos);
}

inline struct Vec3i vec3i_init(const struct Vec3& pos)
{
    struct Vec3i p = {{{ int(pos.x), int(pos.y), int(pos.z) }}};
    return p;
}
