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
const int min_lip = 2; // minimum lip

const int fixed_hall_wid = cubes_across_room / 4;
const int fixed_hall_offs = (cubes_across_room - fixed_hall_wid) / 2; // hall offset
const int fixed_stair_x = 6;
const int fixed_stair_y = 7;
const int fixed_stair_w = 4;
const int fixed_stair_d = 2;



enum direction_t {
    DIR_NORTH, DIR_SOUTH,
    DIR_EAST, DIR_WEST,
    DIR_UP, DIR_DOWN,
	DIR_COUNT // use only as a max value when iterating thru all directions
};
        
enum direction_type_t {
    DIRTYPE_OPENAIR,
    DIRTYPE_HALL,
    DIRTYPE_DOOR,
	DIRTYPE_STAIRS,
    // if editing this, keep in mind code makes random openings assuming 0-2 are valid openings
    DIRTYPE_BLOCKED_BY_ROOM, // ..... and this is used as the excluded max for openings
    DIRTYPE_BLOCKED_BY_OUTSIDE,
    DIRTYPE_BLOCKED_FOREVER, // stops connecting to upper part of large room like Boss Room, or treating stairs same as lateral connections
};

struct Room {
    direction_type_t dir_types[DIR_COUNT];
	int wall_block;
	int floor_block;

    int x_offs;
    int y_offs;
    int wid; // width
    int dep; // depth
    int hei; // height
    int e_hall_hei;
    int e_hall_wid;
    int e_hall_offs; // east hall offset
    int n_hall_hei;
    int n_hall_wid;
    int n_hall_offs; // north hall offset
	int air_x; // pos of a region that guarantees airspace, atm only used for stairways
	int air_y;
	int air_w; // width of air region
	int air_d; // depth
};

Room rooms[rooms_going_up][rooms_across_ruins][rooms_across_ruins];



void set_region(int i_x, int i_y, int i_z, int i_w, int i_dep, int i_h, int tile_id = 1) {
	for (int z = i_z; z < i_z + i_h; z++) {
		for (int y = i_y; y < i_y + i_dep; y++) {
			for (int x = i_x; x < i_x + i_w; x++) {
				t_map::set(x, y, z, tile_id);
			}
		}
	}
}

bool far_north_cube(int y) {
    return y == cubes_across_room - 1;
}
bool far_south_cube(int y) {
    return y == 0;
}
bool far_east_cube(int x) {
    return x == cubes_across_room - 1;
}
bool far_west_cube(int x) {
    return x == 0;
}



bool w_of_n_opening(int rx, int ry, int rz, int cx) {
    return
        cx < rooms[rz][ry][rx].n_hall_offs;
}
bool e_of_n_opening(int rx, int ry, int rz, int cx) {
    return
        cx >= rooms[rz][ry][rx].n_hall_offs
        /**/ + rooms[rz][ry][rx].n_hall_wid;
}

bool s_of_e_opening(int rx, int ry, int rz, int cy) {
    return
        cy < rooms[rz][ry][rx].e_hall_offs;
}
bool s_edge_of_e_opening(Room r, int cx, int cy, int cz) {
    return
		cy == r.e_hall_offs - 1 && cz <= r.e_hall_hei; // && far_east_cube(cx); shouldn't be needed now that halls use min_lip
}

bool n_of_e_opening(int rx, int ry, int rz, int cy) {
    return
        cy >= rooms[rz][ry][rx].e_hall_offs
        /**/ + rooms[rz][ry][rx].e_hall_wid;
}

bool not_in_n_hall(int rx, int ry, int rz, int cx) {
    return
        w_of_n_opening(rx, ry, rz, cx) ||
        e_of_n_opening(rx, ry, rz, cx);
}
bool not_in_e_hall(int rx, int ry, int rz, int cy) {
    return
        s_of_e_opening(rx, ry, rz, cy) ||
        n_of_e_opening(rx, ry, rz, cy);
}

bool opens_to(direction_t dir, int rx, int ry, int rz) {
    return rooms[rz][ry][rx].dir_types[dir] < DIRTYPE_BLOCKED_BY_ROOM;
}

bool in_air_region(Room r, int x, int y) {
	//if (x < r.air_x || x >= r.air_x + r.air_w) return false;
	//if (y < r.air_y || y >= r.air_y + r.air_d) return false;
	if (x >= r.air_x && x < r.air_x + r.air_w &&
		y >= r.air_y && y < r.air_y + r.air_d) return true;
    return false;
}



void make_walls_or_airspace(int rx, int ry, int rz, int ox, int oy) { // room indexes, origin
	for (int cx = 0; cx < cubes_across_room; cx++) {
    for (int cy = 0; cy < cubes_across_room; cy++) {
    for (int cz = 0; cz < cubes_going_up - 2; cz++) {
		Room r = rooms[rz][ry][rx];
		int block = r.wall_block;
		int need_airspace = true;

        // do east
        int mid = fixed_hall_offs + fixed_hall_wid; // cubes_across_room / 2;
        if (opens_to(DIR_SOUTH, rx, ry, rz))
            mid = rooms[rz][ry - 1][rx].n_hall_offs + rooms[rz][ry - 1][rx].n_hall_wid;
        if (cx >= mid /*&& cy < fixed_hall_offs*/) {
            switch (r.dir_types[DIR_EAST]) {
                case DIRTYPE_HALL:
					if (far_east_cube(cx) && cz >= r.e_hall_hei) { // make blocks above opening
						need_airspace = false;
						if (cz == r.e_hall_hei && 
							!s_of_e_opening(rx, ry, rz, cy) &&
							!n_of_e_opening(rx, ry, rz, cy) )
							block = r.floor_block;
					}

                    if (s_of_e_opening(rx, ry, rz, cy + min_lip) || 
						(s_of_e_opening(rx, ry, rz, cy) && far_east_cube(cx))) need_airspace = false; /////////////////
					if (s_edge_of_e_opening(r, cx, cy, cz) || 
						(cx == mid && opens_to(DIR_SOUTH, rx, ry, rz) && cz <= rooms[rz][ry-1][rx].n_hall_hei)
						) block = r.floor_block; break;
                case DIRTYPE_DOOR:
					if (far_east_cube(cx) && cz >= r.e_hall_hei) { // make blocks above opening
						need_airspace = false;
						if (cz == r.e_hall_hei && 
							!s_of_e_opening(rx, ry, rz, cy) &&
							!n_of_e_opening(rx, ry, rz, cy) )
							block = r.floor_block;
					}

                    if (far_east_cube(cx) || far_south_cube(cy)) {
                        if (s_of_e_opening(rx, ry, rz, cy)) need_airspace = false;
						if (s_edge_of_e_opening(r, cx, cy, cz) || cx == mid) block = r.floor_block; } break;
                default: // all blockers PLUS currently open air space connection
                    if (cx >= r.x_offs + r.wid /*far_east_cube(cx)*/ || far_south_cube(cy))
                        need_airspace = false; break;
            }
        }

        // do west
        mid = fixed_hall_offs; // cubes_across_room / 2;
        if (opens_to(DIR_NORTH, rx, ry, rz))
            mid = r.n_hall_offs;
        if (cx < mid /*&& cy >= fixed_hall_offs + fixed_hall_wid*/) {
            switch (r.dir_types[DIR_WEST]) {
                case DIRTYPE_HALL:
					if (far_west_cube(cx) && cz >= rooms[rz][ry][rx-1].e_hall_hei)  need_airspace = false; // make blocks above opening
                    if (n_of_e_opening(rx - 1, ry, rz, cy)) need_airspace = false; break;
                case DIRTYPE_DOOR:
					if (far_west_cube(cx) && cz >= rooms[rz][ry][rx-1].e_hall_hei)  need_airspace = false; // make blocks above opening
                    if (far_west_cube(cx) || far_north_cube(cy))
                        if (n_of_e_opening(rx - 1, ry, rz, cy)) need_airspace = false; break;
                default: // all blockers PLUS currently open air space connection
                    if (cx < r.x_offs /*far_west_cube(cx)*/ || far_north_cube(cy))
                        need_airspace = false; break;
            }
        }

        // do north
        mid = fixed_hall_offs + fixed_hall_wid; // cubes_across_room / 2;
        if (opens_to(DIR_EAST, rx, ry, rz))
            mid = r.e_hall_offs + r.e_hall_wid;
        if (cy >= mid /*&& cy >= fixed_hall_offs + fixed_hall_wid*/) {
            switch (r.dir_types[DIR_NORTH]) {
                case DIRTYPE_HALL:
					if (far_north_cube(cy) && cz >= r.n_hall_hei)  need_airspace = false; // make blocks above opening
                    if (e_of_n_opening(rx, ry, rz, cx)) need_airspace = false; break;
                case DIRTYPE_DOOR:
					if (far_north_cube(cy) && cz >= r.n_hall_hei)  need_airspace = false; // make blocks above opening
                    if (far_north_cube(cy) || far_east_cube(cx))
                        if (e_of_n_opening(rx, ry, rz, cx)) need_airspace = false; break;
                default: // all blockers PLUS currently open air space connection
                    if (cy >= r.y_offs + r.dep /*far_north_cube(cy)*/ || far_east_cube(cx))
                        need_airspace = false; break;
            }
        }

        // do south
        mid = fixed_hall_offs + fixed_hall_wid; // cubes_across_room / 2;
        if (opens_to(DIR_WEST, rx, ry, rz))
            mid = rooms[rz][ry][rx - 1].e_hall_offs;
        if (cy < mid /*&& cy < fixed_hall_offs*/) {
            switch (r.dir_types[DIR_SOUTH]) {
                case DIRTYPE_HALL:
					if (far_south_cube(cy) && cz >= rooms[rz][ry-1][rx].n_hall_hei)  need_airspace = false; // make blocks above opening
                    if (w_of_n_opening(rx, ry-1, rz, cx)) need_airspace = false; break;
                case DIRTYPE_DOOR:
					if (far_south_cube(cy) && cz >= rooms[rz][ry-1][rx].n_hall_hei)  need_airspace = false; // make blocks above opening
                    if (far_south_cube(cy) || far_west_cube(cx))
                        if (w_of_n_opening(rx, ry - 1, rz, cx)) need_airspace = false; break;
                default: // all blockers PLUS currently open air space connection
                    if (cy < r.y_offs /*far_south_cube(cy)*/ || far_west_cube(cx))
                        need_airspace = false; break;
            }
        }

		if (in_air_region(rooms[rz][ry][rx], cx, cy)) need_airspace = true; 

		// add 4 to all z values, to get above bedrock
		if (need_airspace) t_map::set(rx * cubes_across_room + cx + ox, ry * cubes_across_room + cy + oy, rz * cubes_going_up + cz + 4, 0);
		else               t_map::set(rx * cubes_across_room + cx + ox, ry * cubes_across_room + cy + oy, rz * cubes_going_up + cz + 4, block);
	}
	}
    }
}

void make_stairs(int rx, int ry, int rz, int ox, int oy, int floor_block) { // room indexes, origin
	set_region(
		rx * cubes_across_room + ox + fixed_stair_x,
		ry * cubes_across_room + oy + fixed_stair_y,
		rz * cubes_going_up + 3 + 1,
		1, fixed_stair_d, 2, floor_block);
	set_region(
		rx * cubes_across_room + ox + fixed_stair_x + 1,
		ry * cubes_across_room + oy + fixed_stair_y,
		rz * cubes_going_up + 3 + 2,
		1, fixed_stair_d, 3, floor_block);
	set_region(
		rx * cubes_across_room + ox + fixed_stair_x + 2,
		ry * cubes_across_room + oy + fixed_stair_y,
		rz * cubes_going_up + 3 + 4,
		1, fixed_stair_d, 3, floor_block);
	set_region(
		rx * cubes_across_room + ox + fixed_stair_x + 3,
		ry * cubes_across_room + oy + fixed_stair_y,
		rz * cubes_going_up + 3 + 6,
		1, fixed_stair_d, 3, floor_block);
}

Room setup_stairs_for(direction_t d, Room r) {
	r.dir_types[d] = DIRTYPE_STAIRS;
	r.air_x = fixed_stair_x - 2;
	r.air_y = fixed_stair_y - 2;
	r.air_w = fixed_stair_w + 4;
	r.air_d = fixed_stair_d + 4;
	return r;
}

void setup_rooms() {
    for (int z = 0; z < rooms_going_up; z++) {
		// floors have 1 stairway up
		int stairway_up_x = randrange(0, rooms_across_ruins - 1);
		int stairway_up_y = randrange(0, rooms_across_ruins - 1);

		for (int x = 0; x < rooms_across_ruins; x++) {
		for (int y = 0; y < rooms_across_ruins; y++) {
			Room r;
			r.floor_block = randrange(33, 40);
			do {
				r.wall_block = randrange(33, 40);
			} while (r.floor_block == r.wall_block);

			// spans refer to the AIRSPACE, and don't include outer shell of blocks
			// but offset, for cleaner comparisons, should actually be the absolute offset from the corner of the room (including shell)
			int malleable_x_span = cubes_across_room - 2 /* shell of 2 walls */;
			int malleable_y_span = cubes_across_room - 2 /* shell of 2 walls */;
			int malleable_z_span = cubes_going_up - 2 /* shell of 2 walls */;
			r.wid = randrange(malleable_x_span / 2, malleable_x_span);
			r.dep = randrange(malleable_y_span / 2, malleable_y_span);
			r.hei = randrange(2 + min_lip, malleable_z_span);
			malleable_x_span -= r.wid;
			malleable_y_span -= r.dep;
			malleable_z_span -= r.hei;
			r.x_offs = 1 /* shell */ + randrange(0, malleable_x_span);
			r.y_offs = 1 /* shell */ + randrange(0, malleable_y_span);

			//r.e_hall_hei = randrange(2, malleable_z_span);
			//r.n_hall_hei = randrange(2, malleable_z_span);
			r.e_hall_hei = randrange(3, 4);
			r.n_hall_hei = randrange(3, 4);


			// now that i chose my offset, it could have eaten into MALLEABLE span, and i don't think i'm considering that here!
			//.... shouldn't even be using that var?  i'm working within the WID/DEP space when doing the hallways right?!

			// reset malleables, for working INSIDE AIRSPACE
			malleable_x_span = r.wid - min_lip * 2;
			malleable_y_span = r.dep - min_lip * 2;
			r.e_hall_wid = /*fixed_hall_wid; */ randrange(2 /* min opening */, malleable_y_span);
			r.n_hall_wid = /*fixed_hall_wid; */ randrange(2 /* min opening */, malleable_x_span);
			malleable_x_span -= r.n_hall_wid;
			malleable_y_span -= r.e_hall_wid;
			r.e_hall_offs = /*fixed_hall_offs; */ r.y_offs + min_lip + randrange(0, malleable_y_span);
			r.n_hall_offs = /*fixed_hall_offs; */ r.x_offs + min_lip + randrange(0, malleable_x_span);

			// connections in directions
			r.air_x = r.air_y =	r.air_w = r.air_d = 0; // must set some default values since we keep reusing the rooms array

			for (int i = 0; i < DIR_COUNT; i++) {
				if /* lateral dir */ (i < DIR_UP) 
					r.dir_types[i] = (direction_type_t)randrange(1, 2); // randomly choose door or hall
				else if /* stairway up should be here */ (i == DIR_UP && z < rooms_going_up - 1 && x == stairway_up_x && y == stairway_up_y)
					r = setup_stairs_for(DIR_UP, r);
				else {
					r.dir_types[i] = DIRTYPE_BLOCKED_FOREVER;
				}
			}

			if /* stairs going upwards in room below */ (z > 0 && rooms[z - 1][y][x].dir_types[DIR_UP] == DIRTYPE_STAIRS)
				r = setup_stairs_for(DIR_DOWN, r);

			if (y == 0)
				r.dir_types[DIR_SOUTH] = DIRTYPE_BLOCKED_BY_OUTSIDE;
			if (y == rooms_across_ruins - 1)
				r.dir_types[DIR_NORTH] = DIRTYPE_BLOCKED_BY_OUTSIDE;
			if (x == 0)
				r.dir_types[DIR_WEST] = DIRTYPE_BLOCKED_BY_OUTSIDE;
			if (x == rooms_across_ruins - 1)
				r.dir_types[DIR_EAST] = DIRTYPE_BLOCKED_BY_OUTSIDE;
                
			rooms[z][y][x] = r;
		}
		}
	}
}
	
void make_ruins(int x, int y) {
	setup_rooms();

	for (int rx = 0; rx < rooms_across_ruins; rx++) {
    for (int ry = 0; ry < rooms_across_ruins; ry++) {
    for (int rz = 0; rz < rooms_going_up; rz++) {
		int ceil_block = randrange(33, 40);

		// make floor 
		set_region(
			rx * cubes_across_room + x,
			ry * cubes_across_room + y,
			rz * cubes_going_up + 3,
			cubes_across_room, cubes_across_room, 1, rooms[rz][ry][rx].floor_block);
		
		// make ceiling
		set_region(
			rx * cubes_across_room + x,
			ry * cubes_across_room + y,
			rz * cubes_going_up + 3 + cubes_going_up - 1,
			cubes_across_room, cubes_across_room, 1, ceil_block);
		
		make_walls_or_airspace(rx, ry, rz, x, y);
		
		if (opens_to(DIR_UP, rx, ry, rz) ) 
			make_stairs(rx, ry, rz, x, y, rooms[rz][ry][rx].floor_block);

		if (opens_to(DIR_DOWN, rx, ry, rz) ) 
			// clear well in floor of this room, and ceiling of room underneath
			set_region(
				rx * cubes_across_room + x + fixed_stair_x,
				ry * cubes_across_room + y + fixed_stair_y,
				rz * cubes_going_up + 3 - 1,
				fixed_stair_w, fixed_stair_d, 2, 0);
    }
    }
	}
}

void generate_ruins()
{
    printf("Making ruins\n");

    for (int x = 0; x < ruins_across_world; x++)
    for (int y = 0; y < ruins_across_world; y++)
		if (x % 2 == 0  &&  y % 2 == 0)
			if (randrange(0, 1) == 0)
				make_ruins(
					x * cubes_across_room * rooms_across_ruins, 
					y * cubes_across_room * rooms_across_ruins);
}

//bool not_in_hall(int i, int z) {
//	if (z >= fixed_hall_wid ||
//		i < fixed_hall_offs || 
//		i >= fixed_hall_offs + fixed_hall_wid) return true;
//	return false;
//}

}   // t_gen
