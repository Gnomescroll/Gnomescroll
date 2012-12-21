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

	int plant_1 = t_mech::get_mech_type("acadia_flower_stage_1");
	int plant_2 = t_mech::get_mech_type("acadia_flower_stage_2");
	int plant_3 = t_mech::get_mech_type("acadia_flower_stage_3");

	int plant_4 = t_mech::get_mech_type("cryptid_larvae_stage_1");
	int plant_5 = t_mech::get_mech_type("cryptid_larvae_stage_2");

	int i = 0;
	int plants_max = 4096;
	int tries = 0;

    int regolith = t_map::get_cube_id("regolith");


    int plant_num = 0;
    while(plant_num < plants_max)
    {

    	if(tries > 4096*8)
    	{
    		printf("WARNING t_gen::place_native_plants: max tries reached \n");
    		break;
    	}

    	int x = genrand_int32 % 512;
    	int y = genrand_int32 % 512;

    	int bid;
    	int pbid = 0;

    	for(int z=127; z>0; z--)
    	{
    		bid = t_map::t_map::get(x,y,z);

    		if(bid == regolith)
    		{
    			if(pbid != 0)
    			{
    				tries++
    				continue;
    			}

    			create_mech(x,y,z, plant_1);
    			plant_num++;
    			continue;
    		}

    	}

    }

//create_mech(int x, int y, int z, MechType mech_type);


}


}