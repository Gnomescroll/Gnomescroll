#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>


namespace t_gen 
{



class ruinsSeg;
class ruinsSeg {
	int x;
	int y;
	int z;
	int wid;
	int dep;
	int hei;
	bool is_hallway;

	ruinsSeg() {
		x = wid = 0;
		y = dep = 0;
		z = hei = 0;
		is_hallway = false;
	}
};

bool within_door_space(int c_z, int door_h, int dimension);
bool within_door_space(int c_z, int door_h, int dimension) {
	return c_z <= door_h &&  
		dimension < door_center + door_radius && 
		dimension > door_center - door_radius;
}
	
void start_dungeon_generator()
{
    printf("Carving out ruins\n");
	ruinsSeg ruins [ZMAX, YMAX, XMAX];
	int span = 16;
	int min_lip = 2;
	int c_x = 0; // current index in current cell
	int c_y = 0;
	int c_z = 0;
	int door_radius = randrange(1 /* causes min door width to be 3 */, span - min_lip * 2);
	int door_center = randrange(min_lip + door_radius, span - min_lip - door_radius);
	int door_h = /**/ randrange(min_lip, span - min_lip); // height

	for (int x = 0; x < XMAX; x++) 
	{
		for (int y = 0; y < YMAX; y++) 
		{
			for (int z = 3; z < ZMAX / 2; z++)  // bedrock overwites below 3
			{
				
				if (c_x == 0)  // @ a west wall block
				{
					if (t_map::get(x,y,z))  // if not airspace
						t_map::set(x, y, z, 1/*tile_id*/);   // create wall

					if (within_door_space(c_z, door_h, y) )
						t_map::set(x, y, z, 0/*tile_id*/);  // clear doorway
				}
				else
				if (c_y == 0)  // @ a north wall block
				{
					if (t_map::get(x,y,z))  // if not airspace
						t_map::set(x, y, z, 1/*tile_id*/);   // create wall

					if (within_door_space(c_z, door_h, x) )
						t_map::set(x, y, z, 0/*tile_id*/);  // clear doorway
				}
				else
				if (c_z == 0)  // @ a down wall (or floor) block
				{
					if (t_map::get(x,y,z))  // if not airspace
						t_map::set(x, y, z, 1/*tile_id*/);   // create block
				}
				else  // clear cell filling
				{
					t_map::set(x, y, z, 0/*tile_id*/); 
				}

				c_z++;
				if (c_z == span) 
					c_z = 0;
			}

			c_y++;
			if (c_y == span) 
				c_y = 0;
		}

		c_x++;
		if (c_x == span) 
			c_x = 0;
	}

	/*
	//get block id from block name
	int tile_id = t_map::dat_get_cube_id("block_name");

	//clears block at x,y,z
	t_map::set(x,y,z, 0); 

	//set block at x,y,z
	t_map::set(x,y,z, tile_id); 

	//get block at x,y,z

	int id = t_map::get(x,y,z)
	*/
}


}   // t_gen
