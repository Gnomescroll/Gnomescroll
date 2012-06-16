#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

namespace t_gen
{

void generate_ore_vein(int x, int y, int z, int size, int tile_id);

void populate_ore_veins(int number, const char* block_name)
{
	int tile_id = t_map::dat_get_cube_id(block_name);

	for(int i=0; i<number; i++)
	{
		int x = genrand_int32() % 512;
		int y = genrand_int32() % 512;
		int z = genrand_int32() % 128;

		int ctile = t_map::get(x,y,z);
		if(ctile == 0) continue;

		int s = genrand_int32() % 128;
		generate_ore_vein(x,y,z, 2*s+1, tile_id);
	}
}

void populate_ore()
{
	populate_ore_veins(4096, "methane_ice");

	populate_ore_veins(2048, "graphite");
	populate_ore_veins(2048, "iron_ore");
	populate_ore_veins(2048, "copper_ore");
	populate_ore_veins(2048, "gallium_ore");
	populate_ore_veins(2048, "iridium_ore");
}

/*
	Add branching ore veins
*/

void generate_ore_vein(int x, int y, int z, int size, int tile_id)
{
	const static int_fast8_t s_array[18] = {
	        0,0,1,  //top
	        0,0,-1, //bottom
	        1,0,0,  //north
	        -1,0,0, //south
	        0,1,0,  //west
	        0,-1,0, //east
	        };

	int tries = 0;

	while(size != 0 && tries < 10)
	{
		int direction = genrand_int32() % 6;

		int cx = x+s_array[3*direction+0];
		int cy = y+s_array[3*direction+1];
		int cz = z+s_array[3*direction+2];

		int ctile = t_map::get(cx,cy,cz);

		if(ctile == 0)
		{
			tries++;
			continue;
		}

		if(ctile == tile_id)
		{
			x = cx;
			y = cy;
			z = cz;
			tries++;
			continue;
		}

		x = cx;
		y = cy;
		z = cz;

		t_map::set(x,y,z,tile_id);

		size--;
	}
}


}