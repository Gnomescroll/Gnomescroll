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
    Vec3i p;
    for(p.x = position.x - radius; p.x <= position.x + radius; p.x++)
    for(p.y = position.y - radius; p.y <= position.y + radius; p.y++)
    for(p.z = position.z - radius; p.z <= position.z + radius; p.z++)
    {
        if (sqrtf(powf(position.x - p.x, 2) + powf(position.y - p.y, 2) + powf(position.z - p.z, 2)) <= radius)
            t_map::set(translate_position(p), material);
    }
}

void generate_line(int startx, int starty, int startz, int endx, int endy, int endz, CubeType material, int thickness)
{
    const int distance = sqrtf(powf(startx - endx, 2) + powf(starty - endy, 2) + powf(startz - endz, 2));
    int minx;
    int maxx;
    int miny;
    int maxy;
    int minz;
    int maxz;
    if(startx < endx)
    {
        minx = startx;
        maxx = endx;
    }
    else
    {
        minx = endx;
        maxx = startx;
    }
    if(starty < endy)
    {
        miny = starty;
        maxy = endy;
    }
    else
    {
        miny = endy;
        maxy = starty;
    }
    if(startz < endz)
    {
        minz = startz;
        maxz = endz;
    }
    else
    {
        minz = endz;
        maxz = startz;
    }
    for(int x = minx; x <= maxx; x++)
    for(int y = miny; y <= maxy; y++)
    for(int z = minz; z <= maxz; z++)
    {
        if(sqrtf(powf(startx - x, 2) + powf(starty - y, 2) + powf(startz - z, 2)) + sqrtf(powf(endx - x, 2) + powf(endy - y, 2) + powf(endz - z, 2)) <= distance + thickness) t_map::set(x, y, z, material);
    }
}

}
