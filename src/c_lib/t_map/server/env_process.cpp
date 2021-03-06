/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "env_process.hpp"

namespace t_map
{

#if DC_SERVER

static const int RAS = 571; //random array size
int* _random = NULL;

void init_env_process()
{
    GS_ASSERT(_random == NULL);
    _random = (int*)malloc(RAS*sizeof(int));
    for (int i=0; i<RAS; i++) _random[i] = rand();
}

void teardown_env_process()
{
    free(_random);
}

OPTIMIZED
void environment_process_tick()
{
    GS_ASSERT(_random != NULL);
    if (_random == NULL) return;

    static int regolith_id = get_cube_type("regolith");

    static int _random_index = rand();
    _random_index = (_random_index+rand()) % RAS;

    static int x=0;
    static int y=0;
    static int z=0;

    for (int i=0; i<7919; i++)
    {
        struct MapElement e1 = get_element(x,y,z);

        if (e1.block == regolith_id)
        {
            //flip pallete if there is empty space above regolith
            if (e1.palette == 0)
            {
                struct MapElement e2 = get_element(x,y,z+1);

                if (isOccludes((CubeType)e2.block) == 0)
                {
                    _random_index = (_random_index+1) % RAS;
                    if (_random[_random_index] % 7 == 0) //14% chance
                    {
                        broadcast_set_block_palette(x,y,z, (CubeType)e1.block, 1); //setting regolith
                    }
                }
            }
            //flip pallete if there is no space above regolith
            else if (e1.palette == 1)
            {
                struct MapElement e2 = get_element(x,y,z+1);

                if (isOccludes((CubeType)e2.block) == 1)
                {
                    _random_index = (_random_index+1) % RAS;
                    if (_random[_random_index] % 3 == 0) //33% chance
                    {
                        broadcast_set_block_palette(x,y,z, (CubeType)e1.block, 0); //setting regolith
                    }
                }
            }

        }

        x++;
        if (x >= 512)
        {
            x=0;
            y++;
            if (y >= 512)
            {
                y=0;
                z++;
                if (z >= 128)
                {
                    z = 0;
                }
            }
        }
    }
}

//run at startup
OPTIMIZED
void environment_process_startup()
{
    int regolith_id = get_cube_type("regolith");
    int s_time = _GET_MS_TIME();

    for (int z=0; z<map_dim.z; z++)
    for (int x=0; x<map_dim.x; x++)
    for (int y=0; y<map_dim.y; y++)
    {
        struct MapElement e1 = get_element(x,y,z);

        if (e1.block == regolith_id)
        {
            //flip pallete if there is empty space above regolith
            if (e1.palette == 0)
            {
                struct MapElement e2 = get_element(x,y,z+1);

                if (isOccludes((CubeType)e2.block) == 0)
                {
                    broadcast_set_block_palette(x,y,z, (CubeType)e1.block, 1); //setting regolith
                }
            }
            //flip pallete if there is no space above regolith
            else if (e1.palette == 1)
            {
                struct MapElement e2 = get_element(x,y,z+1);

                if (isOccludes((CubeType)e2.block) == 1)
                {
                    broadcast_set_block_palette(x,y,z, (CubeType)e1.block, 0); //setting regolith
                }
            }

        }
    }



    printf("t_map::environment_process_startup took %i ms\n", _GET_MS_TIME()-s_time);

}

#endif



}   // t_map
