#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

namespace climate
{

int get_temperature(int x, int y, int z)
{
    GS_ASSERT(x < map_dim.x);
    GS_ASSERT(x >= 0);
    GS_ASSERT(y < map_dim.y);
    GS_ASSERT(y >= 0);
    GS_ASSERT(z < map_dim.z);
    GS_ASSERT(z >= 0);
    if (y > map_dim.y / 2)
        return (map_dim.y - y + x % 16 - x % 21) / 4 - z / 32;
    else if (y <= map_dim.y / 2)
        return (y - x % 16 - x % 21) / 4 - z / 32;
}

int get_oxygen(int x, int y, int z)
{
    GS_ASSERT(x < map_dim.x);
    GS_ASSERT(x >= 0);
    GS_ASSERT(y < map_dim.y);
    GS_ASSERT(y >= 0);
    GS_ASSERT(z < map_dim.z);
    GS_ASSERT(z >= 0);
    if (y > map_dim.y / 2)
        return map_dim.y - y + y % x + x % y - z / 8;
    else if (y <= map_dim.y / 2)
        return y + y % x + x % y - z / 8;
}

int get_nitrogen(int x, int y, int z)
{
    GS_ASSERT(x < map_dim.x);
    GS_ASSERT(x >= 0);
    GS_ASSERT(y < map_dim.y);
    GS_ASSERT(y >= 0);
    GS_ASSERT(z < map_dim.z);
    GS_ASSERT(z >= 0);
    if (y > map_dim.y / 2)
        return map_dim.y - y + 32 - x %  32 - z / 16;
    else if (y <= map_dim.y / 2)
        return y + 32 - x % 32 - z / 16;
}

int get_methane(int x, int y, int z)
{
    GS_ASSERT(x < map_dim.x);
    GS_ASSERT(x >= 0);
    GS_ASSERT(y < map_dim.y);
    GS_ASSERT(y >= 0);
    GS_ASSERT(z < map_dim.z);
    GS_ASSERT(z >= 0);
    if (x > map_dim.x / 2)
        return x - map_dim.x / 2 + z / 32 + y % 4;
    else if (x <= map_dim.x / 2)
        return map_dim.x / 2 - x + z / 32 - y % 4;
}

}   // climate
