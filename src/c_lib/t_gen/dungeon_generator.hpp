#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>


namespace t_gen 
{

const int cubes_across_room = 16;
const int cubes_going_up = cubes_across_room / 2;
const int rooms_across_ruins = XMAX / cubes_across_room;
const int rooms_going_up = 4; // 4 dungeon levels should keep most of the dungeon underground?
const int fixed_hall_wid = cubes_across_room / 4;
const int fixed_hall_offs = (cubes_across_room - fixed_hall_wid) / 2; // hall offset
// const int min_lip = 2; // corpusc paradigm

struct Room {
	bool is_hallway;
	int block;

	// for corpusc's unfinished paradigm
    int x_offs; 
    int y_offs;
    int wid; // width
    int dep; // depth
    int hei; // height
    int e_hall_wid;
    int e_hall_offs; // east hall offset
    int n_hall_wid;
    int n_hall_offs; // north hall offset
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
	if (z > fixed_hall_wid ||
		i < fixed_hall_offs || 
		i >= fixed_hall_offs + fixed_hall_wid) return true;
	return false;
}

void make_stairs(int rx, int ry, int rz, int floor_block) {
		set_region(
			6 + rx * cubes_across_room,
			7 + ry * cubes_across_room,
			rz * cubes_going_up + 3 + 1,
			1, 2, 2, floor_block);
		set_region(
			7 + rx * cubes_across_room,
			7 + ry * cubes_across_room,
			rz * cubes_going_up + 3 + 2,
			1, 2, 3, floor_block);
		set_region(
			8 + rx * cubes_across_room,
			7 + ry * cubes_across_room,
			rz * cubes_going_up + 3 + 4,
			1, 2, 3, floor_block);
		set_region(
			9 + rx * cubes_across_room,
			7 + ry * cubes_across_room,
			rz * cubes_going_up + 3 + 6,
			1, 2, 3, floor_block);
}

void start_dungeon_generator()
{
    printf("Carving out ruins\n");
	Room rooms[rooms_across_ruins][rooms_across_ruins];

    // corpusc paradigm
	//// setup room settings
	////			todo: replace all numerous occurrences of "rooms[y][x]" with a one letter placeholder for neatness? 
	//for (int x = 0; x < rooms_across_ruins; x++) {
 //   for (int y = 0; y < rooms_across_ruins; y++) {
 //       rooms[y][x].wid = randrange(cubes_going_up, cubes_across_room);
 //       rooms[y][x].dep = randrange(cubes_going_up, cubes_across_room);
 //       rooms[y][x].hei = randrange(cubes_going_up, cubes_across_room);
 //       int malleable_x_span /* (room) */ = rooms[y][x].wid - 2 /* shell of 2 walls */ - min_lip * 2;
 //       int malleable_y_span /* (room) */ = rooms[y][x].dep - 2 /* shell of 2 walls */ - min_lip * 2;
 //       rooms[y][x].x_offs = randrange(0, cubes_across_room - rooms[y][x].wid);
 //       rooms[y][x].y_offs = cubes_across_room - rooms[y][x].dep;
 //       rooms[y][x].e_hall_wid = randrange(2 /* min width */, malleable_y_span);
 //       rooms[y][x].e_hall_offs = rooms[y][x].y_offs + min_lip + 1 + randrange(0, malleable_y_span - rooms[y][x].e_hall_wid);
 //       rooms[y][x].n_hall_wid = randrange(2 /* min width */, malleable_x_span);
 //       rooms[y][x].n_hall_offs = rooms[y][x].x_offs + min_lip + 1 + randrange(0, malleable_x_span - rooms[y][x].n_hall_wid);
 //   }
 //   }

	// make rooms
    for (int rx = 0; rx < rooms_across_ruins; rx++) {
    for (int ry = 0; ry < rooms_across_ruins; ry++) {
    for (int rz = 0; rz < rooms_going_up; rz++) {
		int wall_block = randrange(31, 44);
		int floor_block = randrange(31, 44);
		int ceil_block = randrange(31, 44);

		//floor
		set_region(
			rx * cubes_across_room,
			ry * cubes_across_room,
			rz * cubes_going_up + 3,
			cubes_across_room, cubes_across_room, 1, floor_block);
		// ceiling
		set_region(
			rx * cubes_across_room,
			ry * cubes_across_room,
			rz * cubes_going_up + 3 + cubes_going_up,
			cubes_across_room, cubes_across_room, 1, ceil_block);
		// clear out stairwell hole in ceiling
		set_region(
			6 + rx * cubes_across_room,
			7 + ry * cubes_across_room,
			rz * cubes_going_up + 3,
			4, 2, 1, 0);

		// make walls and clear out airspace
		for (int cx = 0; cx < cubes_across_room; cx++) {
        for (int cy = 0; cy < cubes_across_room; cy++) {
        for (int cz = 0; cz < cubes_going_up - 1; cz++) {
			int need_airspace = true;
			
            if /* left edge */ (cx == 0) {
                if (rx == 0 || not_in_hall(cy, cz) ) 
					// cy < rooms[ry, rx - 1]->e_hall_offs || cy >= rooms[ry, rx - 1]->e_hall_offs + rooms[ry, rx - 1]->e_hall_wid)
                    need_airspace = false;
            }
            if (cx == cubes_across_room - 1) {
                if (rx == rooms_across_ruins - 1 || not_in_hall(cy, cz) ) 
					// cy < rooms[ry, rx]->e_hall_offs || cy >= rooms[ry, rx]->e_hall_offs + rooms[ry, rx]->e_hall_wid)
                    need_airspace = false;
            }
            if (cy == 0) {
                if (ry == 0 || not_in_hall(cx, cz) ) 
					// cx < rooms[ry - 1, rx]->n_hall_offs || cx >= rooms[ry - 1, rx]->n_hall_offs + rooms[ry - 1, rx]->n_hall_wid)
                    need_airspace = false;
            }
            if (cy == cubes_across_room - 1) {
                if (ry == rooms_across_ruins - 1 || not_in_hall(cx, cz) ) 
					// cx < rooms[ry, rx]->n_hall_offs || cx >= rooms[ry, rx]->n_hall_offs + rooms[ry, rx]->n_hall_wid)
                    need_airspace = false;
            }

			// add 4 to all z values, to get above bedrock
			if (need_airspace) 
                t_map::set(rx * cubes_across_room + cx, ry * cubes_across_room + cy, rz * cubes_going_up + cz + 4, 0);
			else
                t_map::set(rx * cubes_across_room + cx, ry * cubes_across_room + cy, rz * cubes_going_up + cz + 4, wall_block);
		}
		}
        }

		make_stairs(rx, ry, rz, floor_block);
    }
    }
	}
}

}   // t_gen
