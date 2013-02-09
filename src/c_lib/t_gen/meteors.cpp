#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
#include <t_gen/meteors.hpp>

namespace t_gen
{

void meteor_fall()
{
    static const CubeType bedrock = t_map::get_cube_type("bedrock");
    IF_ASSERT(!t_map::isValidCube(bedrock)) return;

    static CubeType rock = t_map::get_cube_type("rock");
    IF_ASSERT(!t_map::isValidCube(rock)) return;

    const int METEOR_SIZE = 5; //one side of the meteor, which is a cube - will make it have a random shape later
    const int displacement = 200; //the amount of additional blocks of the ore to place around the crater

    int x = randrange(65, t_map::map_dim.x - METEOR_SIZE - 65);
    int y = randrange(65, t_map::map_dim.y - METEOR_SIZE - 65);
    x = translate_point(x);
    y = translate_point(y);

    int z = t_map::get_highest_open_block(x, y) - METEOR_SIZE * randrange(2, 3);
    if (z <= 0) return;

    int xcurrent = x;
    int ycurrent = y;
    int zcurrent = z;

    CubeType tile_id = NULL_CUBE;
    switch (randrange(1,7))
    {
        case 1:
            tile_id = t_map::get_cube_type("iron_ore");
            break;
        case 2:
            tile_id = t_map::get_cube_type("coal");
            break;
        case 3:
            tile_id = t_map::get_cube_type("copper_ore");
            break;
        case 4:
            tile_id = t_map::get_cube_type("iridium_ore");
            break;
        case 5:
            tile_id = t_map::get_cube_type("gallium_ore");
            break;
        case 6:
            tile_id = t_map::get_cube_type("methane_ice");
            break;
        case 7:
            tile_id = t_map::get_cube_type("silicon_ore");
            break;
        default:
            GS_ASSERT(false);
            return;
    }
    IF_ASSERT(!t_map::isValidCube(tile_id)) return;

    //#if !PRODUCTION
    //const char* cube_name = t_map::get_cube_name(tile_id);
    //GS_ASSERT(cube_name != NULL);
    //if (cube_name != NULL)
    //    printf("Incoming %s meteor at %d, %d, %d!\n", cube_name, x, y, z);
    //#endif

    for(int i = x; i <= x + METEOR_SIZE; i++) //generate the actual meteor
    for(int j = y; j <= y + METEOR_SIZE; j++)
    for(int k = z; k <= z + METEOR_SIZE; k++)
    {
        if ((k & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if(randrange(0, 10) < 9) t_map::set(i, j, k, tile_id);
        else t_map::set(i, j, k, rock); //add impurities
    }

    for(int i = x; i <= x + METEOR_SIZE; i++) //degenerate the main hole in the ground
    for(int j = y; j <= y + METEOR_SIZE; j++)
    {
        if (((z + METEOR_SIZE + 1) & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        degenerate_column(i, j, z + METEOR_SIZE + 1);
    }

    //generate the parts that make the meteor seem round, called them rounders, they have more impurities since their impact strength must have bee the highest

    for(int i = x + randrange(1, 2); i <= x + METEOR_SIZE - randrange(1, 2); i++) //generate the upper rounder of the meteor-rounders make stuff look round
    for(int j = y + randrange(1, 2); j <= y + METEOR_SIZE - randrange(1, 2); j++)
    {
        if (((z + METEOR_SIZE + 1) & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if(randrange(0, 5) < 3) t_map::set(i, j, z + METEOR_SIZE + 1, tile_id);
        else t_map::set(i, j, z + METEOR_SIZE + 1, rock); //more impurities here
    }

    for(int i = x + randrange(1, 2); i <= x + METEOR_SIZE - randrange(1, 2); i++) //generate the lower rounder of the meteor
    for(int j = y + randrange(1, 2); j <= y + METEOR_SIZE - randrange(1, 2); j++)
    {
        if (((z - 1) & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if(randrange(0, 5) < 2) t_map::set(i, j, z - 1, tile_id);
        else t_map::set(i, j, z - 1, rock); //even more impurities here, because the impact strength must have been epic
    }

    for(int i = x + randrange(1, 2); i <= x + METEOR_SIZE - randrange(1, 2); i++) //generate another rounder of the meteor
    for(int k = z + randrange(1, 2); k <= z + METEOR_SIZE - randrange(1, 2); k++)
    {
        if ((k & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if(randrange(0, 5) < 2) t_map::set(i, y - 1, k, tile_id);
        else t_map::set(i, y - 1, k, rock); //more impurities here, again
    }

    for(int i = x + randrange(1, 2); i <= x + METEOR_SIZE - randrange(1, 2); i++) //generate another rounder of the meteor
    for(int k = z + randrange(1, 2); k <= z + METEOR_SIZE - randrange(1, 2); k++)
    {
        if ((k & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if(randrange(0, 5) < 2) t_map::set(i, y + 1 + METEOR_SIZE, k, tile_id);
        else t_map::set(i, y + METEOR_SIZE + 1, k, rock); //more impurities here, too
    }

    for(int j = y + randrange(1, 2); j <= y + METEOR_SIZE - randrange(1, 2); j++) //generate another rounder of the meteor
    for(int k = z + randrange(1, 2); k <= z + METEOR_SIZE - randrange(1, 2); k++)
    {
        if ((k & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if(randrange(0, 5) < 2) t_map::set(x - 1, j, k, tile_id);
        else t_map::set(x - 1, j, k, rock); //more impurities here, again
    }

    for(int j = y + randrange(1, 2); j <= y + METEOR_SIZE - randrange(1, 2); j++) //generate another rounder of the meteor
    for(int k = z + randrange(1, 2); k <= z + METEOR_SIZE - randrange(1, 2); k++)
    {
        if ((k & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if(randrange(0, 5) < 2) t_map::set(x + METEOR_SIZE + 1, j, k, tile_id);
        else t_map::set(x + 1 + METEOR_SIZE, j, k, rock); //more impurities here, again
    }

    //Now generate the debris and crater

    for (int count=0; count < displacement; count++)
    {
        zcurrent = randrange(z + METEOR_SIZE, z + METEOR_SIZE * 3);
        xcurrent = randrange(x + (zcurrent - z) - METEOR_SIZE * 5, x - (zcurrent - z) + METEOR_SIZE * 6);
        xcurrent = translate_point(xcurrent);
        ycurrent = randrange(y + (zcurrent - z) - METEOR_SIZE * 5, y - (zcurrent - z) + METEOR_SIZE * 6);
        ycurrent = translate_point(ycurrent);
        degenerate_column(xcurrent, ycurrent, zcurrent + 1);
        if (zcurrent > t_map::get_highest_open_block(xcurrent, ycurrent)) zcurrent = t_map::get_highest_open_block(xcurrent, ycurrent);
        if ((zcurrent & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) continue;   // stop the assert
        if (randrange(1, 5) == 1) t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
        else t_map::set(xcurrent, ycurrent, zcurrent, rock); //lots of impurities here because the crater is mainly made out of stuff that the meteor pushed away from itself
    }

    //now degenerate the blocks above the side rounders(not all rounders will be uncovered, but that brings some randomness)
    for(int i = x + randrange(1, 2); i <= x + METEOR_SIZE - randrange(1, 2); i++)
    {
        degenerate_column(i, y - 1, z + METEOR_SIZE);
    }

    for(int i = x + randrange(1, 2); i <= x + METEOR_SIZE - randrange(1, 2); i++)
    {
        degenerate_column(i, y + METEOR_SIZE + 1, z + METEOR_SIZE);
    }

    for(int j = y + randrange(1, 2); j <= y + METEOR_SIZE - randrange(1, 2); j++)
    {
        degenerate_column(x - 1, j, z + METEOR_SIZE);
    }

    for(int j = y + randrange(1, 2); j <= y + METEOR_SIZE - randrange(1, 2); j++)
    {
        degenerate_column(x + METEOR_SIZE + 1, j, z + METEOR_SIZE);
    }

}

void meteor_shower()
{
    int AMOUNT = randrange(5, 20);
    int killed_blocks;
    int x;
    int y;
    int z;
    CubeType bedrock = t_map::get_cube_type("bedrock");
    CubeType steelA = t_map::get_cube_type("steel_block_1");
    CubeType steelB = t_map::get_cube_type("steel_block_2");
    CubeType steelC = t_map::get_cube_type("steel_block_3");
    CubeType material;

    switch (randrange(1, 7))
    {
        case 1:
            material = t_map::get_cube_type("iron_ore");
            break;
        case 2:
            material = t_map::get_cube_type("coal");
            break;
        case 3:
            material = t_map::get_cube_type("copper_ore");
            break;
        case 4:
            material = t_map::get_cube_type("iridium_ore");
            break;
        case 5:
            material = t_map::get_cube_type("gallium_ore");
            break;
        case 6:
            material = t_map::get_cube_type("methane_ice");
            break;
        case 7:
            material = t_map::get_cube_type("silicon_ore");
            break;
        default:
            GS_ASSERT(false);
            return;
    }

    for(int count = 1; count <= AMOUNT; count++)
    {
        x = randrange(1, t_map::map_dim.x - 1);
        y = randrange(1, t_map::map_dim.y - 1);
        z = t_map::map_dim.z - 1;
        #if !PRODUCTION
        //printf("Creating meteor from meteor shower at %d, %d \n", x, y);
        #endif
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

void degenerate_column(int x, int y, int z)
{
    x = translate_point(x);
    y = translate_point(y);
    for(int k = z; k < t_map::map_dim.z; k++)
    {
        t_map::set(x, y, k, EMPTY_CUBE);
    }
}

}   // t_gen
