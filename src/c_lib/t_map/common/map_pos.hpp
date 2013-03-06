#pragma once

#include <t_map/common/types.hpp>
#include <physics/quadrant.hpp>

// TODO -- replace all with a generic Vec3i

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
    struct MapPos p = { x, y, z };
    return p;
}

inline struct MapPos map_pos_init(const struct MapPos& pos)
{
    struct MapPos p = { pos.x, pos.y, pos.z };
    return p;
}

inline struct MapPos map_pos_init(const struct Vec3& pos)
{
    struct MapPos p = { int(pos.x), int(pos.y), int(pos.z) };
    return p;
}

inline struct MapPosOffset map_offset_init(int d)
{
    struct MapPosOffset m = { d, d, d };
    return m;
}

inline struct MapPosOffset map_offset_init(int d, int e, int f)
{
    struct MapPosOffset m = { d, e, f };
    return m;
}

inline struct MapPos add_offset(struct MapPos pos, const struct MapPosOffset& off)
{
    pos.x = translate_point(pos.x + off.x);
    pos.y = translate_point(pos.y + off.y);
    pos.z = pos.z + off.z;
    return pos;
}
