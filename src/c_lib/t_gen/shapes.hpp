#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/_interface.hpp>
#include <t_map/config/block_dat.hpp>

namespace t_gen
{

void generate_sphere(const struct Vec3i& position, int radius, CubeType material)
{
    const int radius_sq = radius * radius;
    Vec3i p;
    for (p.x = position.x - radius; p.x <= position.x + radius; p.x++)
    for (p.y = position.y - radius; p.y <= position.y + radius; p.y++)
    for (p.z = position.z - radius; p.z <= position.z + radius; p.z++)
        if (vec3i_distance_squared(position, p) <= radius_sq)
            t_map::set(translate_position(p), material);
}

void generate_line(Vec3i start, Vec3i end, CubeType material, int thickness)
{
    Vec3i min = vec3i_min(start, end);
    Vec3i max = vec3i_max(start, end);
    min.x = translate_point(min.x - thickness);
    min.y = translate_point(min.y - thickness);
    min.z = translate_point(min.z - thickness);
    max.x = translate_point(max.x + thickness);
    max.y = translate_point(max.y + thickness);
    max.z = translate_point(max.z + thickness);
    Vec3i p;
    for(p.x = min.x; p.x <= max.x; p.x++)
    for(p.y = min.y; p.y <= max.y; p.y++)
    for(p.z = min.z; p.z <= max.z; p.z++)
        if (sqrtf(vec3i_distance_squared(start, p)) + sqrtf(vec3i_distance_squared(end, p)) < thickness + sqrtf(vec3i_distance_squared(start, end)))
            t_map::set(translate_position(p), material);
}

void generate_line(int a, int b, int c, int x, int y, int z, CubeType material, int thickness)
{
    generate_line(vec3i_init(a, b, c), vec3i_init(x, y, z), material, thickness);
}

}   // t_gen
