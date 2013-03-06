#pragma once

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

inline struct Vec3i vec3i_init(const struct Vec3& pos)
{
    struct Vec3i p = {{{ int(pos.x), int(pos.y), int(pos.z) }}};
    return p;
}

inline struct Vec3i vec3i_add(struct Vec3i pos, const struct Vec3i& off);
