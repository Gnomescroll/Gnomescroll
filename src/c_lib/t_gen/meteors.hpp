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

    int z = randrange (30, 65);
    z = t_map::get_nearest_open_block(x,y,z,1) - 1;
    if (z <= 0) return;

    int xcurrent = x + METEOR_SIZE / 2;
    int ycurrent = y + METEOR_SIZE / 2;
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

    while (1) //while the generation has not been stopped by attempting to overwrite itself, generate the lower half of the meteor
    {
        t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
        if (xcurrent <= x + z + 1.5 * METEOR_SIZE - zcurrent)
        {
            xcurrent++;
        }
        else
        {
            xcurrent = x + z + 0.5 * METEOR_SIZE - zcurrent;
            if (ycurrent <= y + z + 1.5 * METEOR_SIZE - zcurrent)
            {
                ycurrent++;
            }
            else
            {
                ycurrent = y + z + 0.5 * METEOR_SIZE - zcurrent;
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

    while (1) //generate the upper half of the meteor
    {
        t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
        if (xcurrent <= x - (zcurrent - z + METEOR_SIZE) + METEOR_SIZE / 2)
        {
            xcurrent++;
        }
        else
        {
            xcurrent = x - (zcurrent - z + METEOR_SIZE) - METEOR_SIZE / 2;
            if (ycurrent <= y - (zcurrent - z + METEOR_SIZE) + METEOR_SIZE / 2)
            {
                ycurrent++;
            }
            else
            {
                ycurrent = x - (zcurrent - z + METEOR_SIZE) - METEOR_SIZE / 2;
                if (zcurrent <= z + METEOR_SIZE * 2)
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
        if (xcurrent <= x + METEOR_SIZE + zcurrent / 20)
        {
            xcurrent++;
        }
        else
        {
            xcurrent = x;
            if (ycurrent <= y + METEOR_SIZE + zcurrent / 20)
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

void meteor_shower()
{
    int AMOUNT = randrange(5, 20);
    int killed_blocks;
    int x;
    int y;
    int z;
    CubeID bedrock=t_map::get_cube_id("bedrock");
    CubeID steelA=t_map::get_cube_id("steel_block1");
    CubeID steelB=t_map::get_cube_id("steel_block2");
    CubeID steelC=t_map::get_cube_id("steel_block3");
    CubeID material;
    switch (randrange(1, 6))
    {
        case 1:
            material = t_map::get_cube_id("iron_ore");
            printf("Starting iron meteor shower \n");
            break;
        case 2:
            material = t_map::get_cube_id("coal");
            printf("Starting coal meteor shower \n");
            break;
        case 3:
            material = t_map::get_cube_id("copper_ore");
            printf("Starting copper meteor shower \n");
            break;
        case 4:
            material = t_map::get_cube_id("iridium_ore");
            printf("Starting iridium meteor shower \n");
            break;
        case 5:
            material = t_map::get_cube_id("gallium_ore");
            printf("Starting gallium meteor shower \n");
            break;
        case 6:
            material = t_map::get_cube_id("methane_ice");
            printf("Starting methane meteor shower \n");
            break;
        default:
            GS_ASSERT(false);
            return;
    }
    for(int count = 1; count <= AMOUNT; count++)
    {
        printf("Creating meteor from meteor shower at %d, %d \n", x, y);
        x = randrange(1, t_map::map_dim.x - 1);
        y = randrange(1, t_map::map_dim.y - 1);
        z = t_map::map_dim.z - 1;
        printf("Creating meteor from meteor shower at %d, %d \n", x, y);
        killed_blocks = 0;
        while(z > 1 && killed_blocks <= 3)
        {
            if(t_map::get(x, y, z) != EMPTY_CUBE)
            {
                if(t_map::get(x, y, z) == bedrock || t_map::get(x, y, z) == steelA || t_map::get(x, y, z) == steelB || t_map::get(x, y, z) == steelC)
                {
                    killed_blocks=4; //so that further generation is impossible
                }
                if(killed_blocks == 3)
                {
                    t_map::set(x, y, z, material);
                }
                else if (killed_blocks < 3)
                {
                    t_map::set(x, y, z, EMPTY_CUBE);
                }
                else if (killed_blocks > 3)
                {
                    if(z < t_map::map_dim.z - 1)
                    {
                        t_map::set(x, y, z + 1, material);
                    }
                }
                killed_blocks++;
            }
            z = z - 1;
        }
    }
}

}   // t_gen
