#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>


namespace t_gen 
{

//struct RuinsSeg;
struct RuinsSeg {
	int x, y, z, wid, dep, hei;
	bool is_hallway;

	RuinsSeg() {
		x = wid = 
		y = dep =
		z = hei = 0;
		is_hallway = false;
	}
};

void set_region(int i_x, int i_y, int i_z, int i_w, int i_dep, int i_h, int tile_id); 
void set_region(int i_x, int i_y, int i_z, int i_w, int i_dep, int i_h, int tile_id = 1) 
{
	for (int z = i_z; z < i_z + i_h; z++) 
	{
		for (int y = i_y; y < i_y + i_dep; y++) 
		{
			for (int x = i_x; x < i_x + i_w; x++) 
			{
				//if (t_map::get(x, y, z))  // if not airspace
					t_map::set(x, y, z, tile_id);   // create wall
			}
		}
	}
}

void set_region(RuinsSeg rs, int tile_id); 
void set_region(RuinsSeg rs, int tile_id = 1) 
{
	set_region(rs.x, rs.y, rs.z, rs.wid, rs.dep, rs.hei, tile_id);
}

void start_dungeon_generator()
{
	/*
	get block id from block name
	int tile_id = t_map::dat_get_cube_id("block_name");

	clears block at x,y,z
	t_map::set(x,y,z, 0); 

	set block at x,y,z
	t_map::set(x,y,z, tile_id); 

	get block at x,y,z

	int id = t_map::get(x,y,z)
	*/
}


}   // t_gen
