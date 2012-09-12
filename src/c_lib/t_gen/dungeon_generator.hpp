#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>


namespace t_gen 
{

const int ruins_across_world = 8;
const int cubes_across_room = 16;
const int cubes_going_up = cubes_across_room / 2;
const int rooms_across_ruins = XMAX / ruins_across_world / cubes_across_room;
const int rooms_going_up = 5; // levels/floors
const int fixed_hall_wid = cubes_across_room / 4;
const int fixed_hall_offs = (cubes_across_room - fixed_hall_wid) / 2; // hall offset

struct Room {
	bool is_hallway;
	int block;
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
					t_map::set(x, y, z, tile_id);
			}
		}
	}
}

bool not_in_hall(int i, int z) {
	if (z >= fixed_hall_wid ||
		i < fixed_hall_offs || 
		i >= fixed_hall_offs + fixed_hall_wid) return true;
	return false;
}

void make_walls_or_airspace(int rx, int ry, int rz, int ox, int oy, bool halls_wanted) { // room indexes, origin
	int wall_block = randrange(33, 40);

	for (int cx = 0; cx < cubes_across_room; cx++) {
    for (int cy = 0; cy < cubes_across_room; cy++) {
    for (int cz = 0; cz < cubes_going_up - 1; cz++) {
		int need_airspace = true;
			
        if (halls_wanted) {
			if (not_in_hall(cx, cz) && not_in_hall(cy, cz) )
				need_airspace = false;
		} else {
			if /* left edge */ (cx == 0)
				if (rx == 0 || not_in_hall(cy, cz) ) 
					need_airspace = false;
			if /* right edge */ (cx == cubes_across_room - 1)
				if (rx == rooms_across_ruins - 1 || not_in_hall(cy, cz) ) 
					need_airspace = false;
			if /* south edge */ (cy == 0)
				if (ry == 0 || not_in_hall(cx, cz) ) 
					need_airspace = false;
			if /* north edge */ (cy == cubes_across_room - 1)
				if (ry == rooms_across_ruins - 1 || not_in_hall(cx, cz) ) 
					need_airspace = false;
		}

		// add 4 to all z values, to get above bedrock
		if (need_airspace) t_map::set(rx * cubes_across_room + cx + ox, ry * cubes_across_room + cy + oy, rz * cubes_going_up + cz + 4, 0);
		else               t_map::set(rx * cubes_across_room + cx + ox, ry * cubes_across_room + cy + oy, rz * cubes_going_up + cz + 4, wall_block);
	}
	}
    }
}

void make_stairs(int rx, int ry, int rz, int ox, int oy, int floor_block) { // room indexes, origin
		set_region(
			6 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 1,
			1, 2, 2, floor_block);
		set_region(
			7 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 2,
			1, 2, 3, floor_block);
		set_region(
			8 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 4,
			1, 2, 3, floor_block);
		set_region(
			9 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 6,
			1, 2, 3, floor_block);
}

void make_ruins(int x, int y) {
    for (int rx = 0; rx < rooms_across_ruins; rx++) {
    for (int ry = 0; ry < rooms_across_ruins; ry++) {
    for (int rz = 0; rz < rooms_going_up; rz++) {
		int floor_block = randrange(33, 40);
		int ceil_block = randrange(33, 40);

		// make floor    (currently there is only a ceiling (1 cube thickness) between ruins floors....should be easy to fix, but more important stuff to do for now)
		//set_region(
		//	rx * cubes_across_room + x,
		//	ry * cubes_across_room + y,
		//	rz * cubes_going_up + 3,
		//	cubes_across_room, cubes_across_room, 1, floor_block);
		
		// make ceiling
		set_region(
			rx * cubes_across_room + x,
			ry * cubes_across_room + y,
			rz * cubes_going_up + 3 + cubes_going_up,
			cubes_across_room, cubes_across_room, 1, ceil_block);
		
		// clear out stairwell hole in ceiling
		set_region(
			6 + rx * cubes_across_room + x,
			7 + ry * cubes_across_room + y,
			rz * cubes_going_up + 3 + cubes_going_up,
			4, 2, 1, 0);

		bool halls_wanted = randrange(1, 3) == 1;
		make_walls_or_airspace(rx, ry, rz, x, y, halls_wanted);
		if (!halls_wanted) 
			make_stairs(rx, ry, rz, x, y, floor_block);
    }
    }
	}
}

void start_dungeon_generator()
{
    printf("Carving out ruins\n");
	Room rooms[rooms_across_ruins][rooms_across_ruins];

    for (int x = 0; x < ruins_across_world; x++) {
    for (int y = 0; y < ruins_across_world; y++) {
		if (x % 2 == 0  &&  y % 2 == 0)
			make_ruins(
				x * cubes_across_room * rooms_across_ruins, 
				y * cubes_across_room * rooms_across_ruins);
    }
	}
}

}   // t_gen
