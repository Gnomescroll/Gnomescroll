#pragma once

#include <math.h>
#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>

struct Vec3i
{
    union
    {
        int i[3];
        struct
        {
            int x, y, z;
        };
    };
};

inline bool is_equal(const struct Vec3i& a, const struct Vec3i& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

float euclidean_distance_2d(const struct Vec3i& a, const struct Vec3i& b);
int diagonal_distance_2d(const struct Vec3i& a, const struct Vec3i& b);
int manhattan_distance_2d(const struct Vec3i& a, const struct Vec3i& b);
float euclidean_distance(const struct Vec3i& a, const struct Vec3i& b);
int diagonal_distance(const struct Vec3i& a, const struct Vec3i& b);
int manhattan_distance(const struct Vec3i& a, const struct Vec3i& b);

inline void vec3i_print(const struct Vec3i& pos)
{
    printf("Vec3i: %d, %d, %d\n", pos.x, pos.y, pos.z);
}

inline struct Vec3i vec3i_init(int x)
{
    struct Vec3i p = {{{ x, x, x }}};
    return p;
}

inline struct Vec3i vec3i_init(int x, int y, int z)
{
    struct Vec3i p = {{{ x, y, z }}};
    return p;
}

inline struct Vec3i vec3i_init(const struct Vec3i& pos)
{
    struct Vec3i p = {{{ pos.x, pos.y, pos.z }}};
    return p;
}

inline struct Vec3i vec3i_init(const int pos[3])
{
    struct Vec3i p = {{{ pos[0], pos[1], pos[2] }}};
    return p;
}

inline struct Vec3i vec3i_init(const struct Vec3& pos);
inline struct Vec3i vec3i_add(struct Vec3i pos, const struct Vec3i& off);

inline struct Vec3i vec3i_sub(struct Vec3i a, struct Vec3i b)
{
    Vec3i c;
    for (int i=0; i<3; i++)
        c.i[i] = a.i[i] - b.i[i];
    return c;
}

inline struct Vec3i vec3i_abs(struct Vec3i a)
{
    Vec3i b = a;
    for (int i=0; i<3; i++)
        b.i[i] = abs(a.i[i]);
    return b;
}

inline int vec3i_volume(const struct Vec3i& p)
{
    return abs(p.x * p.y * p.z);
}
