#pragma once

#include <t_map/common/types.hpp>
#include <physics/quadrant.hpp>

inline bool is_equal(const struct MapPos& a, const struct MapPos& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

float euclidean_distance_2d(const struct MapPos& a, const struct MapPos& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    return sqrtf(x*x + y*y);
}

int diagonal_distance_2d(const struct MapPos& a, const struct MapPos& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    return GS_MAX(abs(x), abs(y));
}

int manhattan_distance_2d(const struct MapPos& a, const struct MapPos& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    return (abs(x) + abs(y));
}

float euclidean_distance(const struct MapPos& a, const struct MapPos& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    int z = quadrant_translate(b.z, a.z) - b.z;
    return sqrtf(x*x + y*y + z*z);
}

int diagonal_distance(const struct MapPos& a, const struct MapPos& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    int z = quadrant_translate(b.z, a.z) - b.z;
    return GS_MAX(abs(z), GS_MAX(abs(x), abs(y)));
}

int manhattan_distance(const struct MapPos& a, const struct MapPos& b)
{
    int x = quadrant_translate(b.x, a.x) - b.x;
    int y = quadrant_translate(b.y, a.y) - b.y;
    int z = quadrant_translate(b.z, a.z) - b.z;
    return (abs(x) + abs(y) + abs(z));
}

inline void print_pos(const struct MapPos& pos)
{
    printf("MapPos: %d, %d, %d\n", pos.x, pos.y, pos.z);
}

inline struct MapPos map_pos_init(int x, int y, int z)
{
    return { x, y, z };
}

inline struct MapPos map_pos_init(const struct MapPos& pos)
{
    return { pos.x, pos.y, pos.z };
}

