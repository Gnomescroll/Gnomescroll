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
    printf("Carving out ruins\n");

	// setup
	const int span = 16;
	const int min_lip = 2;
	const int lat_seg_ext = XMAX/span*2;  // lateral segment extent (max)
	const int vert_seg_ext = lat_seg_ext/8;  // vertical segment extent (max)
	RuinsSeg ruins 
		[lat_seg_ext]
		[lat_seg_ext]
		[vert_seg_ext];

	// build segments
	for (int x = 0; x < lat_seg_ext; x+=2) 
	{
		for (int y = 0; y < lat_seg_ext; y+=2) 
		{
			for (int z = 0; z < vert_seg_ext; z+=2) 
			{
				// absolute positions (not relative to curr cell)
				int abs_x = x/2*span;
				int abs_y = y/2*span;
				int abs_z = z/2*span;
				
				// this room
				ruins[z][y][x].x = abs_x;
				ruins[z][y][x].y = abs_y;
				ruins[z][y][x].z = abs_z;
				ruins[z][y][x].wid = span;
				ruins[z][y][x].dep = span;
				ruins[z][y][x].hei = span;

				int ti = randrange(1, 3);
				
				// make west wall
				ruins[z][y][x].wid = 1;
				set_region(ruins[z][y][x], ti);
				// make east wall
				set_region(abs_x + span - 1, abs_y, abs_z, 1, span, span, ti);
				// clear a hall in west wall
				set_region(abs_x - 1, abs_y + 6, abs_z + 1, 2, span/2, randrange(2, span - 2), 0);

				//
				//// make negY wall
				set_region(abs_x, abs_y, abs_z, 
					span, 1, span, ti);
				// make posY wall
				set_region(abs_x, abs_y + span - 1, abs_z, 
					span, 1, span, ti);
				// clear negY hallway
				set_region(abs_x + 6, abs_y - 1, abs_z + 1, span/2, 2, randrange(2, span - 2), 0);

				
				// make down wall (floor)
				set_region(abs_x, abs_y, abs_z, 
					span, span, 1, ti);

				// next x hallway
				int hall_w    = randrange(2, span - 2);  // don't want hallways wider than outer cell walls
				int hall_offs = 1;//randrange(1, span - 2 - hall_w);
				ruins[z][y][x+1].wid = 2;  // fixme to extend further into rooms to meet their pushed in walls
				ruins[z][y][x+1].hei = /**/ randrange(0 /* never lip at floor */, span - min_lip);
				ruins[z][y][x+1].dep = hall_w;
				ruins[z][y][x+1].x = abs_x + span - 1;
				ruins[z][y][x+1].y = abs_y + hall_offs;
				ruins[z][y][x+1].z = abs_z;
				ruins[z][y][x+1].is_hallway = true;

				// next y hallway
				hall_w    = randrange(2, span - 2);
				hall_offs = 1;//randrange(1, span - 2 - hall_w);
				ruins[z][y+1][x].wid = hall_w; 
				ruins[z][y+1][x].hei = randrange(0 /* never lip at floor */, span - min_lip);
				ruins[z][y+1][x].dep = 2;  // fixme to extend further into rooms to meet their pushed in walls
				ruins[z][y+1][x].x = abs_x + hall_offs;
				ruins[z][y+1][x].y = abs_y + span - 1;
				ruins[z][y+1][x].z = abs_z;
				ruins[z][y+1][x].is_hallway = true;
			}
		}
	}


	set_region(0,0,74,350,350,2);





	int c_x = 0; // current index in current cell
	int c_y = 0;
	int c_z = 0;
	for (int x = 0; x < lat_seg_ext; x+=2) 
	{
		for (int y = 0; y < lat_seg_ext; y+=2) 
		{
			for (int z = 0; z < vert_seg_ext; z+=2) 
			{
				if (ruins[z][y][x].is_hallway) {

					//if (within_door_space(c_z, door_h, y) )
					//	t_map::set(x, y, z, 0/*tile_id*/);  // clear doorway
				}
				else // it's a room
				{
					// make north wall
					set_region(ruins[z][y][x].x,
						ruins[z][y][x].y,
						ruins[z][y][x].z, 
						ruins[z][y][x].wid, 
						2, 
						ruins[z][y][x].hei);

					// make west wall
					set_region(ruins[z][y][x].x,
						ruins[z][y][x].y,
						ruins[z][y][x].z, 
						2, 
						ruins[z][y][x].dep, 
						ruins[z][y][x].hei);

					// make down wall (floor)
					set_region(ruins[z][y][x].x,
						ruins[z][y][x].y,
						ruins[z][y][x].z, 
						ruins[z][y][x].wid, 
						ruins[z][y][x].dep, 
						2);

					//else  // clear cell filling
					//{
					//	t_map::set(x, y, z, 0/*tile_id*/); 
					//}
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
