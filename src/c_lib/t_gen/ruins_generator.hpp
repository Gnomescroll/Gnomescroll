#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>



namespace t_gen {

const size_t  NUM_FLOOR_CUBES = 4; 
CubeType floors[NUM_FLOOR_CUBES];
const size_t NUM_WALL_CUBES = 4; 
CubeType walls[NUM_WALL_CUBES];
const size_t NUM_CEIL_CUBES = 4; 
CubeType ceils[NUM_CEIL_CUBES];
const size_t NUM_TRIM_CUBES = 4;
CubeType trims[NUM_TRIM_CUBES];

const int MAX_ROOMS_ACROSS = 32; // across ruins
const int HALLWAY_HEIGHT = 3;
const int cubes_across_room = 10;
const int cubes_going_up = 8;
const int rooms_going_up = 3; // levels/floors
const int bedrock_offset = 3;
const int min_lip = 2; // minimum lip

const int fixed_hall_wid = 2;
const int fixed_hall_offs = 4; // hall offset

enum direction_t {
    DIR_NORTH, DIR_SOUTH,
    DIR_EAST, DIR_WEST,
    DIR_UP, DIR_DOWN,
    DIR_MAX // use only as a max value when iterating thru all directions
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

enum room_t {
	ROOMT_NORMAL,
	ROOMT_HALL,
	ROOMT_BOSS_CORNER,
	ROOMT_BOSS_NORMAL,
};
        
CubeType randcube(CubeType arr[], int num) {
    return arr[randrange(0, num - 1)];
}

struct IntVec3 {
    int x;
    int y;
    int z;

    IntVec3() { x = y = z = 0; }
    IntVec3(int x_, int y_, int z_) { 
		x = x_;
		y = y_;
		z = z_; }
};
    
struct Rect {
    int x, y, wid, dep;

    Rect() { x = y = wid = dep = 0; }
    Rect(int x_, int y_, int wid_, int dep_) 
	{ 
		x = x_; 
		y = y_; 
		wid = wid_; 
		dep = dep_; 
	}
};
    
Rect fixed_stair(3, 4, 4, 2);


struct Rect3D {
    int x, y, z, wid, dep, hei;
    Rect3D() { x = y = z = wid = dep = hei = 0; }
};
    
struct Room : Rect3D {
	bool dead;
	room_t room_t;
    CubeType wall;
    CubeType floo;
    CubeType ceil;
    CubeType trim;
    Rect3D nh; // north hall
    Rect3D sh; // south hall
    Rect3D eh; // east hall
    Rect3D wh; // west hall
    Rect air; // a region that guarantees airspace, only used for stairways ATM

	Room() {
		dead = true;
		room_t = ROOMT_NORMAL;
	}
};

Room rooms[rooms_going_up][MAX_ROOMS_ACROSS][MAX_ROOMS_ACROSS];
    
void set_region(int i_x, int i_y, int i_z, int i_w, int i_dep, int i_h, CubeType tile_id)
{
    for (int z = i_z; z < i_z + i_h; z++) {
        for (int y = i_y; y < i_y + i_dep; y++) {
            for (int x = i_x; x < i_x + i_w; x++) {
                t_map::set_fast(x, y, z, tile_id);
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
        cx < rooms[rz][ry][rx].nh.x;
}
bool e_of_n_opening(int rx, int ry, int rz, int cx) {
    return
        cx >= rooms[rz][ry][rx].nh.x
        /**/ + rooms[rz][ry][rx].nh.wid;
}

bool s_of_e_opening(int rx, int ry, int rz, int cy) {
    return
        cy < rooms[rz][ry][rx].eh.y;
}
bool s_edge_of_e_opening(Room r, int cx, int cy, int cz) {
    return
        cy == r.eh.y - 1 && cz <= r.eh.hei; // && far_east_cube(cx); shouldn't be needed now that halls use min_lip
}

bool n_of_e_opening(int rx, int ry, int rz, int cy) {
    return
        cy >= rooms[rz][ry][rx].eh.y
        /**/ + rooms[rz][ry][rx].eh.dep;
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

bool in_air_region(Room r, int x, int y) {
    if (x >= r.air.x && x < r.air.x + r.air.wid &&
        y >= r.air.y && y < r.air.y + r.air.dep) return true;
    return false;
}

bool rect_spans(Rect3D r) // if wid, dep, hei are all 0, it doesn't span any space or represent any space
{
    if (r.wid < 1 &&
        r.dep < 1 &&
        r.hei < 1) 
		return false;

	return true;
}

bool rect_contains(Rect r, int x, int y) {
    if (x >= r.x && x < r.x + r.wid &&
        y >= r.y && y < r.y + r.dep) return true;
    return false;
}
bool rect_contains(Rect3D r, int x, int y, int z) {
    if (x >= r.x && x < r.x + r.wid &&
        y >= r.y && y < r.y + r.dep &&
        z >= r.z && z < r.z + r.hei) return true;
    return false;
}
bool rect_plus_margin_contains(Rect3D r, int mar, int x, int y, int z) {
    if (x >= r.x-mar && x < r.x+mar + r.wid &&
        y >= r.y-mar && y < r.y+mar + r.dep &&
        z >= r.z-mar && z < r.z+mar + r.hei) return true;
    return false;
}

bool valid_room_idx_to(direction_t dir, IntVec3 ri) { // room index
	//// needs to be the dir adjusted indexes, cuz THAT'S what could be out of bounds
	//if (
	//	ri.x < 0 || ri.x >= MAX_ROOMS_ACROSS
	//	ri.y < 0 || ri.y >= MAX_ROOMS_ACROSS
	//	ri.z < 0 || ri.z >= MAX_ROOMS_ACROSS
	//	) 
	//{
	//}
	////// also can get an early out by checking if room in that space is dead.... then its halls are irrel

    for (int i = 0; i < 4; i++) // consider all lateral directions
	{
		switch((direction_t)i) 
		{
			case DIR_NORTH: if (rect_spans(rooms[ri.z][ri.y][ri.x].nh) ) return true; break;
			case DIR_SOUTH: if (!rooms[ri.z][ri.y - 1][ri.x].dead) ; break;
			case DIR_EAST:  if (rect_spans(rooms[ri.z][ri.y][ri.x].eh) ) return true; break;
			case DIR_WEST:  if (!rooms[ri.z][ri.y][ri.x - 1].dead) ; break;
		}
	}
    return false;
}

// params:  room indexes,  origin x/y
void make_room_filling(IntVec3 ri, int ox, int oy) {
    for (int cx = 0; cx < cubes_across_room; cx++) {
    for (int cy = 0; cy < cubes_across_room; cy++) {
    for (int cz = 1; cz < cubes_going_up - 1; cz++) { // excluding floor/ceiling layers
        Room r = rooms[ri.z][ri.y][ri.x];
        CubeType cube = r.wall;
        bool need_cube = false;

		// determine need_cube status
		if (true) // FIXME!!!!!!!!!   ROOMT_HALL == r.room_t && cz > HALLWAY_HEIGHT) // if this is the case, don't give a shit about any directions
		{
			if (cy >= r.y + r.dep // we're north of room space
				&& rect_contains(r.nh, cx, cy, cz) )
				need_cube = true;
			if (cy < r.y          // we're south of room space
				&& rect_contains(r.sh, cx, cy, cz) )
				need_cube = true;
			if (cx >= r.x + r.wid // we're east of room space
				&& rect_contains(r.eh, cx, cy, cz) )
				need_cube = true;
			if (cx < r.x // we're west of room space
				&& rect_contains(r.wh, cx, cy, cz) )
				need_cube = true;
		}
		else
		{
			// north
			if (
					(
						(
							ROOMT_HALL == r.room_t // other types will all have thin walls i believe
							&& 
							cy >= r.y + r.dep // we're north of room space
						)
						||
						far_north_cube(cy) 
					)
					&& 
					(
						rect_spans(r.nh) 
							? (/*cz > HALLWAY_HEIGHT ||*/ cx < r.nh.x || cx >= r.nh.x + r.nh.wid) /* not in opening */ 
							: true // not open, so ignore "opening"
					)
				)
				need_cube = true;
		
			// south
			if (
					(
						(
							ROOMT_HALL == r.room_t // other types will all have thin walls i believe
							&& 
							cy < r.y // we're south of room space
						)
						||
						far_south_cube(cy)
					)
					&& 
					(
						rect_spans(r.sh) 
							? (/*cz > HALLWAY_HEIGHT ||*/ cx < r.sh.x || cx >= r.sh.x + r.sh.wid) /* not in opening */ 
							: true // not open, so ignore "opening"
					)
				)
				need_cube = true;

			// east
			if (
					(
						(
							ROOMT_HALL == r.room_t // other types will all have thin walls i believe
							&& 
							cx >= r.x + r.wid // we're east of room space
						)
						||
						far_east_cube(cx)
					)
					&& 
					(
						rect_spans(r.eh) 
							? (/*cz > HALLWAY_HEIGHT ||*/ cy < r.eh.y || cy >= r.eh.y + r.eh.dep) /* not in opening */ 
							: true // not open, so ignore "opening"
					)
				)
				need_cube = true;

			// west
			if (
					(
						(
							ROOMT_HALL == r.room_t // other types will all have thin walls i believe
							&& 
							cx < r.x // we're west of room space
						)
						||
						far_west_cube(cx)
					)
					&& 
					(
						rect_spans(r.wh) 
							? (/*cz > HALLWAY_HEIGHT ||*/ cy < r.wh.y || cy >= r.wh.y + r.wh.dep) /* not in opening */ 
							: true // not open, so ignore "opening"
					)
				)
				need_cube = true;
		}
		
		
		


        

        // do stairsy stuff
        // clear space
        if (in_air_region(r, cx, cy))    need_cube = false; 

		//if (opens_to(DIR_UP, ri) ) 
  //          make_stairs(rx, ry, rz, x, y, rooms[rz][ry][rx].floo);

  //      if (opens_to(DIR_DOWN, ri) ) {
  //          // make trim cube region to hollow out for stairwell
  //          t_gen::set_region(
  //              rx * cubes_across_room + x + fixed_stair.x - 1,
  //              ry * cubes_across_room + y + fixed_stair.y - 1,
  //              rz * cubes_going_up + bedrock_offset - 1,
		//		fixed_stair.wid + 2, fixed_stair.dep + 2, 2, rooms[rz][ry][rx].trim);
  //          // clear well in floor of this room, and ceiling of room underneath
  //          t_gen::set_region(
  //              rx * cubes_across_room + x + fixed_stair.x,
  //              ry * cubes_across_room + y + fixed_stair.y,
  //              rz * cubes_going_up + bedrock_offset - 1,
  //              fixed_stair.wid, fixed_stair.dep, 2, EMPTY_CUBE);
		//}

		// make the cubes at last		
		if (need_cube) {
            // change rim/frame cubes
            if (cz == 1 ||
				rect_plus_margin_contains(r.nh, 1, cx, cy, cz) || 
                rect_plus_margin_contains(r.sh,   1, cx, cy, cz) || 
                rect_plus_margin_contains(r.eh, 1, cx, cy, cz) || 
                rect_plus_margin_contains(r.wh,   1, cx, cy, cz) 
            ) cube = r.trim;
            
            t_map::set(ri.x * cubes_across_room + cx + ox, ri.y * cubes_across_room + cy + oy, ri.z * cubes_going_up + cz + bedrock_offset, cube); 
        } else {
            t_map::set(ri.x * cubes_across_room + cx + ox, ri.y * cubes_across_room + cy + oy, ri.z * cubes_going_up + cz + bedrock_offset, EMPTY_CUBE);
		}
    }
    }
    }
}

void outside_space_span() {
}

void make_stairs(int rx, int ry, int rz, int ox, int oy, CubeType floo) { // room indexes, origin
    t_gen::set_region(
        rx * cubes_across_room + ox + (cubes_across_room / 2 - 1 /* stair x */),
        ry * cubes_across_room + oy + fixed_stair.y,
        rz * cubes_going_up + 3 + 1,
        1, fixed_stair.dep, 2, floo);
    t_gen::set_region(
        rx * cubes_across_room + ox + fixed_stair.x + 1,
        ry * cubes_across_room + oy + fixed_stair.y,
        rz * cubes_going_up + 3 + 2,
        1, fixed_stair.dep, 3, floo);
    t_gen::set_region(
        rx * cubes_across_room + ox + fixed_stair.x + 2,
        ry * cubes_across_room + oy + fixed_stair.y,
        rz * cubes_going_up + 3 + 4,
        1, fixed_stair.dep, 3, floo);
    t_gen::set_region(
        rx * cubes_across_room + ox + fixed_stair.x + 3,
        ry * cubes_across_room + oy + fixed_stair.y,
        rz * cubes_going_up + 3 + 6,
        1, fixed_stair.dep, 3, floo);
}



Room setup_stairspace_for(direction_t d, Room r) {
    //r.dir_types[d] = DIRTYPE_STAIRS;
    r.air.x = fixed_stair.x - 2;
    r.air.y = fixed_stair.y - 2;
    r.air.wid = fixed_stair.wid + 4;
    r.air.dep = fixed_stair.dep + 4;
    return r;
}

Rect3D get_open_connection(direction_t d)
{
	Rect3D r;
	r.wid = r.dep = 2;
	r.hei = HALLWAY_HEIGHT;

	int co = cubes_across_room / 2 - 1; // connection offset
	switch(d)
	{
		case DIR_NORTH:
		case DIR_SOUTH:
			r.x = co;
			break;
		case DIR_EAST:
		case DIR_WEST:
			r.y = co;
			break;
	}

	switch(d)
	{
		case DIR_NORTH:	r.y = cubes_across_room - 1; break;
		case DIR_EAST:	r.x = cubes_across_room - 1; break;
	}

	r.z = 1;
	return r;
}

void connect_these(IntVec3 src, direction_t d, IntVec3 dst) 
{
		// set vars and make root to hall connections
		switch(d) 
		{
			case DIR_NORTH: 
				dst.x = src.x; dst.y = src.y + 1; dst.z = src.z;  // set offset of destination  
				rooms[src.z][src.y][src.x].nh = get_open_connection(DIR_NORTH);
				rooms[dst.z][dst.y][dst.x].sh = get_open_connection(DIR_SOUTH);
				break;
			case DIR_SOUTH: 
				dst.x = src.x; dst.y = src.y - 1; dst.z = src.z;  // set offset of destination
				rooms[src.z][src.y][src.x].sh = get_open_connection(DIR_SOUTH);
				rooms[dst.z][dst.y][dst.x].nh = get_open_connection(DIR_NORTH);
				break;
			case DIR_EAST:  
				dst.x = src.x + 1; dst.y = src.y; dst.z = src.z;  // set offset of destination
				rooms[src.z][src.y][src.x].eh = get_open_connection(DIR_EAST);
				rooms[dst.z][dst.y][dst.x].wh = get_open_connection(DIR_WEST);
				break;
			case DIR_WEST:  
				dst.x = src.x - 1; dst.y = src.y; dst.z = src.z;  // set offset of destination
				rooms[src.z][src.y][src.x].wh = get_open_connection(DIR_WEST);
				rooms[dst.z][dst.y][dst.x].eh = get_open_connection(DIR_EAST);
				break;
		}
}

IntVec3 root;
IntVec3 hall;
IntVec3 room;
bool empty_lat_space_around(IntVec3 iv) 
{
	// find out how many, and which are valid directions
	int num_choices = 0;
	direction_t choices[4]; // 4 lateral possibilities

    for (int i = 0; i < 4; i++) // consider all directions
	{
		switch((direction_t)i) 
		{
			case DIR_NORTH: if (rooms[iv.z][iv.y + 1][iv.x].dead) choices[num_choices++] = DIR_NORTH; break;
			case DIR_SOUTH: if (rooms[iv.z][iv.y - 1][iv.x].dead) choices[num_choices++] = DIR_SOUTH; break;
			case DIR_EAST:  if (rooms[iv.z][iv.y][iv.x + 1].dead) choices[num_choices++] = DIR_EAST; break;
			case DIR_WEST:  if (rooms[iv.z][iv.y][iv.x - 1].dead) choices[num_choices++] = DIR_WEST; break;
		}
	}

	if (num_choices < 1) return false;
	direction_t dir = choices[randrange(0, num_choices - 1)]; // get random dir
	
	if (/* root passed */ iv.x == root.x && iv.y == root.y && iv.z == root.z) 
	{
		connect_these(root, dir, hall);
		
		//// set vars and make root to hall connections
		//switch(choices[randrange(0, num_choices - 1)]) 
		//{
		//	case DIR_NORTH: 
		//		hall.x = root.x; hall.y = root.y + 1; hall.z = root.z; hall_dir = DIR_NORTH;  // set offset of destination     //FIXME   think hall_dir is never used
		//		rooms[root.z][root.y][root.x].nh = get_open_connection(DIR_NORTH);
		//		rooms[hall.z][hall.y][hall.x].sh = get_open_connection(DIR_SOUTH);
		//		break;
		//	case DIR_SOUTH: 
		//		hall.x = root.x; hall.y = root.y - 1; hall.z = root.z; hall_dir = DIR_SOUTH;  // set offset of destination
		//		rooms[root.z][root.y][root.x].sh = get_open_connection(DIR_SOUTH);
		//		rooms[hall.z][hall.y][hall.x].nh = get_open_connection(DIR_NORTH);
		//		break;
		//	case DIR_EAST:  
		//		hall.x = root.x + 1; hall.y = root.y; hall.z = root.z; hall_dir = DIR_EAST;  // set offset of destination
		//		rooms[root.z][root.y][root.x].eh = get_open_connection(DIR_EAST);
		//		rooms[hall.z][hall.y][hall.x].wh = get_open_connection(DIR_WEST);
		//		break;
		//	case DIR_WEST:  
		//		hall.x = root.x - 1; hall.y = root.y; hall.z = root.z; hall_dir = DIR_WEST; // set offset of destination
		//		rooms[root.z][root.y][root.x].wh = get_open_connection(DIR_WEST);
		//		rooms[hall.z][hall.y][hall.x].eh = get_open_connection(DIR_EAST);
		//		break;
		//}
		return true;
	}
	else 
	if (/* hall passed */ iv.x == hall.x && iv.y == hall.y && iv.z == hall.z) 
	{
		connect_these(hall, dir, room);
		return true;
	}
	else
	{	
		printf("Ruins generator: neither root or hall passed\n", num_choices);
		return false;
	}
}

void UNUSED_make_a_simple_room() 
{
    // spans refer to the AIRSPACE, and don't include outer shell of cubes
    // but offset, for cleaner comparisons, should actually be the absolute offset from the corner of the room (including shell)
    IntVec3 mal_span; // malleable/dynamic space span
    mal_span.x = cubes_across_room - 2 /* shell of 2 walls */ - min_lip * 2;
    mal_span.y = cubes_across_room - 2 /* shell of 2 walls */ - min_lip * 2;
    mal_span.z = cubes_going_up    - 2 /* shell of 2 walls */ - min_lip; // floors of the same height are fine, altho ceiling should be lipped

	// fixme    to provide stairs ONLY in rooms, and choose one AFTER a whole floor is generated
    int stairway_up_x = randrange(0, MAX_ROOMS_ACROSS - 1);
    int stairway_up_y = randrange(0, MAX_ROOMS_ACROSS - 1);


			//// figure lateral opening sizes
   //         if (curr.x != MAX_ROOMS_ACROSS - 1) { 
   //             r.eh.x = cubes_across_room - 1;
   //             r.eh.y = 4;
   //             r.eh.wid = 1;     
   //             r.eh.dep = 2; 
   //         }
   //         if (curr.y != MAX_ROOMS_ACROSS - 1) { 
   //             r.nh.x = 4;     
   //             r.nh.y = cubes_across_room - 1;  
   //             r.nh.wid = 2; 
   //             r.nh.dep = 1;     
   //         }

            //// connections in directions (inclu stairs)
            //for (int i = 0; i < DIR_MAX; i++) {
            //    if /* lateral dir */ (i < DIR_UP) 
            //        r.dir_types[i] = (direction_type_t)randrange(1, 2); // randomly choose door or hall
            //    else if /* stairway up should be here */ (i == DIR_UP && z < rooms_going_up - 1 && x == stairway_up_x && y == stairway_up_y)
            //        r = setup_stairspace_for(DIR_UP, r);
            //    else {
            //        r.dir_types[i] = DIRTYPE_BLOCKED_FOREVER;
            //    }
            //}

            //if /* stairs going upwards in room below */ (z > 0 && rooms[z - 1][y][x].dir_types[DIR_UP] == DIRTYPE_STAIRS)
            //    r = setup_stairspace_for(DIR_DOWN, r);
}

void make_and_setup_new_room(IntVec3 iv) 
{
	Room r;
	r.dead = false;
	r.room_t = ROOMT_NORMAL;
	if (randrange(0,1) == 0)
		r.room_t = ROOMT_HALL;
    r.floo = randcube(floors, NUM_FLOOR_CUBES);//floo;
    r.wall = randcube(walls, NUM_WALL_CUBES);//wall;
	r.ceil = randcube(ceils, NUM_CEIL_CUBES);//ceil;
	r.trim = randcube(trims, NUM_TRIM_CUBES);//trim;

    // offset openings, to get inside the outermost layer of room space
	r.nh.z = 1;
	r.sh.z = 1;
    r.eh.z = 1;
	r.wh.z = 1;
                
    // set subspace of grid node
	// currently its always giving the tiniest "room" possible (hallway dimensions), cuz we just ignore those dimensions for other ROOMT_ stuff
	r.x = r.y = 4;
	r.wid = r.dep = 2;
	rooms[iv.z][iv.y][iv.x] = r;
}

IntVec3 find_valid_root() 
{
	printf("find_valid_root() is doing NOTHING ATM!!!!!!***********\n");
		IntVec3 iv; // dead = true by default, and that can be checked, so that if it isn't an alive room returned, we know no valid roots could be found
	return iv;
}
    
void snake_a_new_path() 
{
    for (int z = 0; z < rooms_going_up; z++) {
		printf("floor == %d ----------------------------------------------------------\n", z);

		// setup floorwide settings
		// this root room will be diff for each floor.  it's the corner of a boss room on floor 0 (which will be a 2x3 joining of grid nodes, prob should exceptionally generate bossroom the moment
						// the corner room is established.  since it would be easy to special case, and we could skip its other nodes in the later generation stage
		root.x = randrange(0, MAX_ROOMS_ACROSS - 1 - 3 /* potential boss room wid */);
		root.y = randrange(0, MAX_ROOMS_ACROSS - 1 - 3 /* potential boss room dep */);
		root.z = z;
		make_and_setup_new_room(root);
		
		CubeType floo = randcube(floors, NUM_FLOOR_CUBES);
		CubeType wall = randcube(walls, NUM_WALL_CUBES);
		CubeType ceil = randcube(ceils, NUM_CEIL_CUBES);
		CubeType trim = randcube(trims, NUM_TRIM_CUBES);

        // for each room desired, keep trying to make a valid hall-then-room train (2 car) of space spans
		int num_desired_rooms = 12;
		for (int i = 0; i < num_desired_rooms; i++) {

			int num_tries = 0;
			if (empty_lat_space_around(root) )
			{
				num_tries++;
				if (num_tries > 4000) {printf(">4000 tries!  I GIVE!\n");break;}
		
				if (empty_lat_space_around(hall) )
				{
					make_and_setup_new_room(hall);
					root.x = room.x;
					root.y = room.y;
					root.z = room.z;
					make_and_setup_new_room(room);
				}
			}
			else // must start a new snake, cuz can't build off ROOT room
			{
				IntVec3 iv = find_valid_root();
				root.x = iv.x;
				root.y = iv.y;
				root.z = iv.z;
				if (rooms[iv.z][iv.y][iv.x].dead) break;
			}
        }
    }
}
    
void make_ruins(int x, int y) {
    printf("    ruin %d, %d\n", x, y);
	//make_outer_shell(x, y);
    //setup_rooms(); // init them as dead
	snake_a_new_path();

    // generate each cube now that connections/openings are all setup
	for (int rx = 0; rx < MAX_ROOMS_ACROSS; rx++) {
    for (int ry = 0; ry < MAX_ROOMS_ACROSS; ry++) {
    for (int rz = 0; rz < rooms_going_up; rz++) {
		if (rooms[rz][ry][rx].dead) continue;

        // make floor crust
		CubeType fl = (randrange(0,7) == 0) ? t_map::get_cube_type("rock") : rooms[rz][ry][rx].floo;
        t_gen::set_region(
            rx * cubes_across_room + x,
            ry * cubes_across_room + y,
            rz * cubes_going_up + bedrock_offset,
            cubes_across_room, cubes_across_room, 1, fl);

        // make ceiling crust
        //t_gen::set_region(
        //    rx * cubes_across_room + x,
        //    ry * cubes_across_room + y,
        //    rz * cubes_going_up + bedrock_offset + cubes_going_up - 1,
        //    cubes_across_room, cubes_across_room, 1, rooms[rz][ry][rx].ceil);
        
        IntVec3 ri; /* room index */ ri.x = rx; ri.y = ry; ri.z = rz;
        make_room_filling(ri, x, y);
    }
    }
    }
}

    void check_textures(CubeType arr[], int num) {
		for (int i = 0; i < num; i++) { 
			GS_ASSERT(t_map::isValidCube(trims[i])); 
			if (!t_map::isValidCube(trims[i])) { printf("** cube id %d invalid ***", trims[i]); return; }
		}
	}



    void generate_ruins() {
        printf("Making ruins\n");

		floors[0] = t_map::get_cube_type("ruins_floor1");
        floors[1] = t_map::get_cube_type("ruins_floor2"); 
        floors[2] = t_map::get_cube_type("ruins_floor3"); 
        floors[3] = t_map::get_cube_type("ruins_floor4"); 

        walls[0] = t_map::get_cube_type("ruins_wall1");
        walls[1] = t_map::get_cube_type("ruins_wall2"); 
        walls[2] = t_map::get_cube_type("ruins_wall3");
        walls[3] = t_map::get_cube_type("ruins_wall4"); 

        ceils[0] = t_map::get_cube_type("ruins_ceiling1");
        ceils[1] = t_map::get_cube_type("ruins_ceiling2"); 
        ceils[2] = t_map::get_cube_type("ruins_ceiling3");
        ceils[3] = t_map::get_cube_type("ruins_ceiling4"); 

        trims[0] = t_map::get_cube_type("ruins_trim1");
        trims[1] = t_map::get_cube_type("ruins_trim2"); 
        trims[2] = t_map::get_cube_type("ruins_trim3");
        trims[3] = t_map::get_cube_type("ruins_trim4"); 

        check_textures(floors, NUM_FLOOR_CUBES);
        check_textures(walls,  NUM_WALL_CUBES);
        check_textures(ceils,  NUM_CEIL_CUBES);
        check_textures(trims,  NUM_TRIM_CUBES);

        // generate ruins
		const int num_ruins = 4;//13;
        for (int i = 0; i < num_ruins; i++)
		{
			make_ruins
			(
				randrange(0, t_map::map_dim.x), 
				randrange(0, t_map::map_dim.y)
			);
		}
    }
}   // t_gen
