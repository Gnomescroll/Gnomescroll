#pragma once

#include <t_mech/_interface.hpp>

#if DC_CLIENT
dont_include_this_file_in_client
#endif

/*
Add native plants
*/

namespace t_gen
{


void place_native_plants()
{
    //acadia_flower_stage_1
    //acadia_flower_stage_1
    //cryptid_larvae_stage_1
    //cryptid_larvae_stage_2

    MechType plant_1 = t_mech::get_mech_type_dat("acadia_flower_stage_0");
    MechType plant_2 = t_mech::get_mech_type_dat("acadia_flower_stage_1");
    MechType plant_3 = t_mech::get_mech_type_dat("acadia_flower_stage_2");

    MechType plant_4 = t_mech::get_mech_type_dat("cryptid_larvae_stage_0");
    MechType plant_5 = t_mech::get_mech_type_dat("cryptid_larvae_stage_3");

    const int PLANTS_MAX = 128;
    const int TRIES_MAX  =  32;
    int tries = 0;

    int regolith = t_map::get_cube_type("regolith");


    int plant_num = 0;
    while (plant_num < PLANTS_MAX)
    {

        if (tries >= TRIES_MAX)
        {
            printf("WARNING t_gen::place_native_plants: max tries reached \n");
            break;
        }

        int x = rand() % map_dim.x;
        int y = rand() % map_dim.y;

        int bid;
        int pbid = 0;

        for (int z=map_dim.z-1; z>=0; z--)
        {
            bid = t_map::get(x,y,z);

            if (bid == regolith)
            {
                if (pbid != 0)
                {
                    tries++;
                    break;
                }
                bool ret = false;

                if (rand()%4 != 0)
                {
                    int v = rand()%4;
                    if (v==0) ret = t_mech::create_mech(x,y,z+1, plant_1);
                    if (v==1) ret = t_mech::create_mech(x,y,z+1, plant_1);
                    if (v==2) ret = t_mech::create_mech(x,y,z+1, plant_2);
                    if (v==3) ret = t_mech::create_mech(x,y,z+1, plant_3);
                }
                else
                {
                    if (rand()%4 != 0)
                    {
                        ret = t_mech::create_mech(x,y,z+1, plant_4);
                    }
                    else
                    {
                        ret = t_mech::create_mech(x,y,z+1, plant_5);
                    }
                }

                if (!ret)
                {
                    tries++;
                    break;
                }
                plant_num++;
                tries = 0;
                break;
            }
            pbid = bid;

        }

        tries++;
    }

    printf("t_gen::place_native_plants, created %i plants \n", plant_num);

//create_mech(int x, int y, int z, MechType mech_type);
}

}   // t_gen
