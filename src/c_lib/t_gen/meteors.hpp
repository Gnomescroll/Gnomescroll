#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>

namespace t_gen
{
    
void meteor_fall(void)
{
    static const CubeID bedrock = t_map::get_cube_id("bedrock");
    GS_ASSERT(t_map::isValidCube(bedrock));
    if (!t_map::isValidCube(bedrock)) return;
    static CubeID rock = t_map::get_cube_id("rock");
    GS_ASSERT(t_map::isValidCube(rock));
    if (!t_map::isValidCube(rock)) return;

    const int METEOR_SIZE = 5; //one side of the meteor, which is a cube - will make it have a random shape later
    const int displacement = 100; //the amount of additional blocks of the ore to place around the crater

    int x = randrange (65, t_map::map_dim.x - METEOR_SIZE - 65);
    int y = randrange (65, t_map::map_dim.y - METEOR_SIZE - 65);
    x = translate_point(x);
    y = translate_point(y);

    int z = randrange (15, 65);
    z = t_map::get_nearest_open_block(x,y,z,1) - 1;
    if (z <= 0) return;

    int xcurrent = x;
    int ycurrent = y;
    int zcurrent = z;

    CubeID tile_id = NULL_CUBE;
    switch (randrange(1,6))
    {
        case 1:
            tile_id = t_map::get_cube_id("iron_ore");
            break;
        case 2:
            tile_id = t_map::get_cube_id("coal");
            break;
        case 3:
            tile_id = t_map::get_cube_id("copper_ore");
            break;
        case 4:
            tile_id = t_map::get_cube_id("iridium_ore");
            break;
        case 5:
            tile_id = t_map::get_cube_id("gallium_ore");
            break;
        case 6:
            tile_id = t_map::get_cube_id("methane_ice");
            break;
        default:
            GS_ASSERT(false);
            return;
    }
    GS_ASSERT(t_map::isValidCube(tile_id));
    if (!t_map::isValidCube(tile_id)) return;
    
    const char* cube_name = t_map::get_cube_name(tile_id);
    GS_ASSERT(cube_name != NULL);
    if (cube_name != NULL)
        printf("Incoming %s meteor at %d, %d, %d!\n", cube_name, x, y, z);
    
    while (1) //while the generation has not been stopped by attempting to overwrite itself
    {
        t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
        if (xcurrent <= x + METEOR_SIZE)
        {
            xcurrent++;
        }
        else
        {
            xcurrent = x;
            if (ycurrent <= y + METEOR_SIZE)
            {
                ycurrent++;
            }
            else
            {
                ycurrent = y;
                if (zcurrent <= z + METEOR_SIZE)
                {
                    zcurrent++;
                }
                else
                {
                    break;
                }
            }
        }
    }
    
    while (zcurrent < t_map::map_dim.z)
    {
        t_map::set(xcurrent, ycurrent, zcurrent, EMPTY_CUBE);
        if (xcurrent <= x + METEOR_SIZE)
        {
            xcurrent++;
        }
        else
        {
            xcurrent = x;
            if (ycurrent <= y + METEOR_SIZE)
            {
                ycurrent++;
            }
            else
            {
                ycurrent = y;
                zcurrent++;
            }
        }

    }

    for (int i=0; i<displacement; i++)
    {
        zcurrent = z + METEOR_SIZE + randrange(1, 15);
        if (zcurrent > 0 && zcurrent < t_map::map_dim.z)
        {
            xcurrent = x - randrange(1, (100 - zcurrent) / 5);
            ycurrent = y - randrange(1, (100 - zcurrent) / 5);
            xcurrent = translate_point(xcurrent);
            ycurrent = translate_point(ycurrent);
            if (t_map::get(xcurrent, ycurrent, zcurrent) != bedrock)
            {
                t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
            }
        }
        
        zcurrent = zcurrent - randrange(0, 2);
        if (zcurrent > 0 && zcurrent < t_map::map_dim.z)
        {
            xcurrent += randrange(0, 3);
            ycurrent += randrange(0, 3);
            xcurrent -= randrange(0, 3);
            ycurrent -= randrange(0, 3);
            xcurrent = translate_point(xcurrent);
            ycurrent = translate_point(ycurrent);
            if (t_map::get(xcurrent, ycurrent, zcurrent) != bedrock)
            {
                t_map::set(xcurrent, ycurrent, zcurrent, rock);
            }
            xcurrent -= randrange(0, 5);
            ycurrent -= randrange(0, 5);
            xcurrent += randrange(0, 5);
            ycurrent += randrange(0, 5);
            xcurrent = translate_point(xcurrent);
            ycurrent = translate_point(ycurrent);
            if (t_map::get(xcurrent, ycurrent, zcurrent) != bedrock)
            {
                t_map::set(xcurrent, ycurrent, zcurrent, EMPTY_CUBE);
            }
        }
    }

    for (int i=0; i<displacement/2; i++)
    {
        zcurrent = randrange(0, METEOR_SIZE + 2);
        if (zcurrent > 0 && zcurrent < t_map::map_dim.z)
        {
            xcurrent = x - randrange(0, 2);
            ycurrent = y - randrange(0, 2);
            xcurrent += randrange(0, METEOR_SIZE + 2);
            ycurrent += randrange(0, METEOR_SIZE + 2);
            xcurrent = translate_point(xcurrent);
            ycurrent = translate_point(ycurrent);
            if (t_map::get(xcurrent, ycurrent, zcurrent) != bedrock)
            {
                t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
            }
        }
    }
    
    zcurrent = z + METEOR_SIZE + 1;
    while (zcurrent < t_map::map_dim.z)
    {
        zcurrent = zcurrent + 1;
        if (zcurrent > 0 && zcurrent < t_map::map_dim.z)
        {
            for (int i=0; i<displacement; i++)
            {
                xcurrent = x - randrange(0, zcurrent / 10);
                ycurrent = y - randrange(0, zcurrent / 10);
                xcurrent += randrange(0, METEOR_SIZE + zcurrent /10);
                ycurrent += randrange(0, METEOR_SIZE + zcurrent /10);
                xcurrent = translate_point(xcurrent);
                ycurrent = translate_point(ycurrent);
                if (t_map::get(xcurrent, ycurrent, zcurrent) != bedrock)
                {
                    t_map::set(xcurrent, ycurrent, zcurrent, EMPTY_CUBE);
                }
            }
        }
    }
}

}   // t_gen
