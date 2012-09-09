#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>


namespace t_gen 
{

const int cubes_across_room = 16;
const int rooms_across_ruins = XMAX / cubes_across_room;
const int min_lip = 2;

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

void start_dungeon_generator()
{
    printf("Carving out ruins\n");
	Room rooms[rooms_across_ruins][rooms_across_ruins];

    // setup rooms
	//			todo: replace all numerous occurrences of "rooms[y][x]" with a one letter placeholder for neatness? 
	for (int x = 0; x < rooms_across_ruins; x++) {
    for (int y = 0; y < rooms_across_ruins; y++) {
		rooms[y][x].block = randrange(31, 44);
        rooms[y][x].wid = randrange(cubes_across_room / 2, cubes_across_room);
        rooms[y][x].dep = randrange(cubes_across_room / 2, cubes_across_room);
        rooms[y][x].hei = randrange(cubes_across_room / 2, cubes_across_room); // VVVVVVVVVVVVVVVVVVVVVVVVV needs to come before below
        int malleable_x_span /* (room) */ = rooms[y][x].wid - 2 /* shell of 2 walls */ - min_lip * 2; // ^^^^^^ needs to come after above
        int malleable_y_span /* (room) */ = rooms[y][x].dep - 2 /* shell of 2 walls */ - min_lip * 2;
        rooms[y][x].x_offs = randrange(0, cubes_across_room - rooms[y][x].wid);
        rooms[y][x].y_offs = cubes_across_room - rooms[y][x].dep;
        //r.Color = getNextCol();
        rooms[y][x].e_hall_wid = randrange(2 /* min width */, malleable_y_span);
        rooms[y][x].e_hall_offs = rooms[y][x].y_offs + min_lip + 1 + randrange(0, malleable_y_span - rooms[y][x].e_hall_wid);
        rooms[y][x].n_hall_wid = randrange(2 /* min width */, malleable_x_span);
        rooms[y][x].n_hall_offs = rooms[y][x].x_offs + min_lip + 1 + randrange(0, malleable_x_span - rooms[y][x].n_hall_wid);
    }
    }

	// time to make the donuts!
    for (int rx = 0; rx < rooms_across_ruins; rx++) {
        for (int ry = 0; ry < 1; ry++) {
			//floor
			set_region(
				rx * cubes_across_room,
				ry * cubes_across_room,
				3, //rz * cubes_across_room,
				cubes_across_room, cubes_across_room, 1, rooms[ry, rx]->block);
			// ceil
			set_region(
				rx * cubes_across_room,
				ry * cubes_across_room,
				3 + 8, //rz * cubes_across_room,
				cubes_across_room, cubes_across_room, 1, rooms[ry, rx]->block);
			// ceil stairwell hole
			set_region(
				6 + rx * cubes_across_room,
				7 + ry * cubes_across_room,
				3 + 8, //rz * cubes_across_room,
				4, 2, 1, 0);
			// stairs
			set_region(
				6 + rx * cubes_across_room,
				7 + ry * cubes_across_room,
				3 + 1, //rz * cubes_across_room,
				1, 2, 2, rooms[ry, rx]->block);
			set_region(
				7 + rx * cubes_across_room,
				7 + ry * cubes_across_room,
				3 + 2, //rz * cubes_across_room,
				1, 2, 3, rooms[ry, rx]->block);
			set_region(
				8 + rx * cubes_across_room,
				7 + ry * cubes_across_room,
				3 + 4, //rz * cubes_across_room,
				1, 2, 3, rooms[ry, rx]->block);
			set_region(
				9 + rx * cubes_across_room,
				7 + ry * cubes_across_room,
				3 + 6, //rz * cubes_across_room,
				1, 2, 3, rooms[ry, rx]->block);

            for (int cx = 0; cx < cubes_across_room; cx++) {
            for (int cy = 0; cy < cubes_across_room; cy++) {
            for (int cz = 0; cz < cubes_across_room / 2; cz++) {
				// add 4 to all z values, to get above bedrock
                if (cx == 0) {
                    if /* leftmost room, or not in hall */ (rx == 0 || cy < rooms[ry, rx - 1]->e_hall_offs || cy >= rooms[ry, rx - 1]->e_hall_offs + rooms[ry, rx - 1]->e_hall_wid)
                        t_map::set(rx * cubes_across_room + cx, ry * cubes_across_room + cy, cz + 4, rooms[ry, rx]->block);
                }
                if (cx == cubes_across_room - 1) {
                    if /* rightmost room, or not in hall */ (rx == rooms_across_ruins - 1 || cy < rooms[ry, rx]->e_hall_offs || cy >= rooms[ry, rx]->e_hall_offs + rooms[ry, rx]->e_hall_wid)
                        t_map::set(rx * cubes_across_room + cx, ry * cubes_across_room + cy, cz + 4, rooms[ry, rx]->block);
                }
                if (cy == 0) {
                    if /* ynegmost room, or not in hall */ (ry == 0 || cx < rooms[ry - 1, rx]->n_hall_offs || cx >= rooms[ry - 1, rx]->n_hall_offs + rooms[ry - 1, rx]->n_hall_wid)
                        t_map::set(rx * cubes_across_room + cx, ry * cubes_across_room + cy, cz + 4, rooms[ry, rx]->block);
                }
                if (cy == cubes_across_room - 1) {
                    if /* yposimost room, or not in hall */ (ry == rooms_across_ruins - 1 || cx < rooms[ry, rx]->n_hall_offs || cx >= rooms[ry, rx]->n_hall_offs + rooms[ry, rx]->n_hall_wid)
                        t_map::set(rx * cubes_across_room + cx, ry * cubes_across_room + cy, cz + 4, rooms[ry, rx]->block);
                }
			}
			}
            }
        }
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
