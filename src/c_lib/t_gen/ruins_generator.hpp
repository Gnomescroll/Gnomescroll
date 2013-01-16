#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>

namespace t_gen {

const size_t    NUM_FLOOR_CUBES = 4; 
CubeType floors[NUM_FLOOR_CUBES];
const size_t    NUM_WALL_CUBES = 4; 
CubeType walls [NUM_WALL_CUBES];
const size_t    NUM_CEIL_CUBES = 4; 
CubeType ceils [NUM_CEIL_CUBES];
const size_t    NUM_TRIM_CUBES = 4;
CubeType trims [NUM_TRIM_CUBES];

const int ROOMS_GOING_ACROSS = 32; // across ruins
const int ROOMS_GOING_UP = 3; // levels/floors
const int HALLWAY_HEIGHT = 3;
const int CUBES_ACROSS_ROOM = 10;
const int CUBES_GOING_UP_ROOM = 8;
const int BEDROCK_OFFSET = 3;
const int MIN_LIP = 2; // minimum lip

const int FIXED_CONNECTION_SPAN = 2;
const int FIXED_CONNECTION_OFFSET = 4; // hall offset

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
    enum room_t room_t;
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

//Room rooms[ROOMS_GOING_UP][ROOMS_GOING_ACROSS][ROOMS_GOING_ACROSS];
Room*** rooms = NULL; //[ROOMS_GOING_UP][ROOMS_GOING_ACROSS][ROOMS_GOING_ACROSS];


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

bool in_air_region(Room r, int x, int y) 
{
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

bool rect_contains(Rect r, int x, int y) 
{
    if (x >= r.x && x < r.x + r.wid &&
        y >= r.y && y < r.y + r.dep) return true;
    return false;
}
bool rect_contains(Rect3D r, int x, int y, int z) 
{
    if (x >= r.x && x < r.x + r.wid &&
        y >= r.y && y < r.y + r.dep &&
        z >= r.z && z < r.z + r.hei) return true;
    return false;
}

bool rect_plus_margin_contains(Rect3D r, int mar, int x, int y, int z) 
{
    if (x >= r.x-mar && x < r.x+mar + r.wid &&
        y >= r.y-mar && y < r.y+mar + r.dep &&
        z >= r.z-mar && z < r.z+mar + r.hei) return true;
    return false;
}

// params:  room indexes,  origin x/y
void make_room_filling(IntVec3 ri, int ox, int oy) 
{
    for (int cx = 0; cx < CUBES_ACROSS_ROOM; cx++)
    for (int cy = 0; cy < CUBES_ACROSS_ROOM; cy++)
    for (int cz = 1; cz < CUBES_GOING_UP_ROOM - 1; cz++) { // excluding floor/ceiling layers
        Room r = rooms[ri.z][ri.y][ri.x];
        CubeType cube = r.wall;
        bool need_cube = false;

        // determine need_cube status
        if (cy >= r.y + r.dep // we're north of room space
            && !rect_contains(r.nh, cx, cy, cz) )
            need_cube = true;
        if (cy < r.y          // we're south of room space
            && !rect_contains(r.sh, cx, cy, cz) )
            need_cube = true;
        if (cx >= r.x + r.wid // we're east of room space
            && !rect_contains(r.eh, cx, cy, cz) )
            need_cube = true;
        if (cx < r.x // we're west of room space
            && !rect_contains(r.wh, cx, cy, cz) )
            need_cube = true;

        // do stairsy stuff
        // clear space
        //if (in_air_region(r, cx, cy))    need_cube = false; 

        //if (opens_to(DIR_UP, ri) ) 
  //          make_stairs(rx, ry, rz, x, y, rooms[rz][ry][rx].floo);

  //      if (opens_to(DIR_DOWN, ri) ) {
  //          // make trim cube region to hollow out for stairwell
  //          t_gen::set_region(
  //              rx * CUBES_ACROSS_ROOM + x + fixed_stair.x - 1,
  //              ry * CUBES_ACROSS_ROOM + y + fixed_stair.y - 1,
  //              rz * CUBES_GOING_UP_ROOM + BEDROCK_OFFSET - 1,
        //      fixed_stair.wid + 2, fixed_stair.dep + 2, 2, rooms[rz][ry][rx].trim);
  //          // clear well in floor of this room, and ceiling of room underneath
  //          t_gen::set_region(
  //              rx * CUBES_ACROSS_ROOM + x + fixed_stair.x,
  //              ry * CUBES_ACROSS_ROOM + y + fixed_stair.y,
  //              rz * CUBES_GOING_UP_ROOM + BEDROCK_OFFSET - 1,
  //              fixed_stair.wid, fixed_stair.dep, 2, EMPTY_CUBE);
        //}

        // make the cubes at last       
        if (need_cube) {
            // change rim/frame cubes
            if (cz == 1 ||
                rect_plus_margin_contains(r.nh, 1, cx, cy, cz) || 
                rect_plus_margin_contains(r.sh, 1, cx, cy, cz) || 
                rect_plus_margin_contains(r.eh, 1, cx, cy, cz) || 
                rect_plus_margin_contains(r.wh, 1, cx, cy, cz) 
            ) 
				cube = r.trim;
            
            t_map::set(ri.x * CUBES_ACROSS_ROOM + cx + ox, ri.y * CUBES_ACROSS_ROOM + cy + oy, ri.z * CUBES_GOING_UP_ROOM + cz + BEDROCK_OFFSET, cube); 
        } else {
            t_map::set(ri.x * CUBES_ACROSS_ROOM + cx + ox, ri.y * CUBES_ACROSS_ROOM + cy + oy, ri.z * CUBES_GOING_UP_ROOM + cz + BEDROCK_OFFSET, EMPTY_CUBE);
        }
    }
}

void outside_space_span() {
}

void make_stairs(int rx, int ry, int rz, int ox, int oy, CubeType floo) { // room indexes, origin
    t_gen::set_region(
        rx * CUBES_ACROSS_ROOM + ox + (CUBES_ACROSS_ROOM / 2 - 1 /* stair x */),
        ry * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        rz * CUBES_GOING_UP_ROOM + 3 + 1,
        1, fixed_stair.dep, 2, floo);
    t_gen::set_region(
        rx * CUBES_ACROSS_ROOM + ox + fixed_stair.x + 1,
        ry * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        rz * CUBES_GOING_UP_ROOM + 3 + 2,
        1, fixed_stair.dep, 3, floo);
    t_gen::set_region(
        rx * CUBES_ACROSS_ROOM + ox + fixed_stair.x + 2,
        ry * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        rz * CUBES_GOING_UP_ROOM + 3 + 4,
        1, fixed_stair.dep, 3, floo);
    t_gen::set_region(
        rx * CUBES_ACROSS_ROOM + ox + fixed_stair.x + 3,
        ry * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        rz * CUBES_GOING_UP_ROOM + 3 + 6,
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

bool valid_room_idx_to_dir_from(direction_t dir, IntVec3 from) { // room index
	IntVec3 to;

    switch(dir) 
    {
        case DIR_NORTH: 
            to.x = from.x;
			to.y = from.y + 1; 
			to.z = from.z;  // set offset of destination  
			break;
        case DIR_SOUTH: 
            to.x = from.x; 
			to.y = from.y - 1; 
			to.z = from.z;  // set offset of destination
            break;
        case DIR_EAST:  
            to.x = from.x + 1; 
			to.y = from.y; 
			to.z = from.z;  // set offset of destination
            break;
        case DIR_WEST:  
            to.x = from.x - 1; 
			to.y = from.y; 
			to.z = from.z;  // set offset of destination
            break;

        case DIR_UP:
        case DIR_DOWN:
        case DIR_MAX:
            GS_ASSERT(false);
            break;
    }

    if (
      to.x < 0 || to.x >= ROOMS_GOING_ACROSS
	 ||
      to.y < 0 || to.y >= ROOMS_GOING_ACROSS
	 ||
      to.z < 0 || to.z >= ROOMS_GOING_UP
      ) 
    {
		return false;
    }

    return true;
}

void open_connection_to(direction_t d, Room& rm)
{
    switch(d)
    {
        case DIR_NORTH:
			rm.nh.x = FIXED_CONNECTION_OFFSET;
			rm.nh.y = rm.y + rm.dep;
			rm.nh.wid = FIXED_CONNECTION_SPAN;
			rm.nh.dep = CUBES_ACROSS_ROOM / 2;  // longer than needed
			break;
        case DIR_SOUTH:
            rm.sh.x = FIXED_CONNECTION_OFFSET;
			rm.sh.y = 0;
			rm.sh.wid = FIXED_CONNECTION_SPAN;
			rm.sh.dep = CUBES_ACROSS_ROOM / 2;  // longer than needed
            break;
        case DIR_EAST:
			rm.eh.x =  rm.x + rm.wid;
			rm.eh.y = FIXED_CONNECTION_OFFSET;
			rm.eh.wid = CUBES_ACROSS_ROOM / 2;  // longer than needed
			rm.eh.dep = FIXED_CONNECTION_SPAN;
            break;
        case DIR_WEST:
			rm.wh.x =  0;
			rm.wh.y = FIXED_CONNECTION_OFFSET;
			rm.wh.wid = CUBES_ACROSS_ROOM / 2;  // longer than needed
			rm.wh.dep = FIXED_CONNECTION_SPAN;
            break;

        case DIR_UP:
        case DIR_DOWN:
        case DIR_MAX:
            GS_ASSERT(false);
            break;
    }
}

void connect_these(IntVec3 src, direction_t d, IntVec3& dst) 
{
        // set vars and make root to hall connections
        switch(d) 
        {
            case DIR_NORTH: 
                dst.x = src.x;
				dst.y = src.y + 1; 
				dst.z = src.z;  // set offset of destination  

				open_connection_to(DIR_NORTH, rooms[src.z][src.y][src.x]);
				open_connection_to(DIR_SOUTH, rooms[dst.z][dst.y][dst.x]);
				break;
            case DIR_SOUTH: 
                dst.x = src.x; 
				dst.y = src.y - 1; 
				dst.z = src.z;  // set offset of destination

				open_connection_to(DIR_SOUTH, rooms[src.z][src.y][src.x]);
				open_connection_to(DIR_NORTH, rooms[dst.z][dst.y][dst.x]);
                break;
            case DIR_EAST:  
                dst.x = src.x + 1; 
				dst.y = src.y; 
				dst.z = src.z;  // set offset of destination

				open_connection_to(DIR_EAST, rooms[src.z][src.y][src.x]);
				open_connection_to(DIR_WEST, rooms[dst.z][dst.y][dst.x]);
                break;
            case DIR_WEST:  
                dst.x = src.x - 1; 
				dst.y = src.y; 
				dst.z = src.z;  // set offset of destination

				open_connection_to(DIR_WEST, rooms[src.z][src.y][src.x]);
				open_connection_to(DIR_EAST, rooms[dst.z][dst.y][dst.x]);
                break;

            case DIR_UP:
            case DIR_DOWN:
            case DIR_MAX:
                GS_ASSERT(false);
                break;
        }
}

bool idx_passed_was(IntVec3 iv) 
{
    //if (/* root passed */ iv.x == root.x && iv.y == root.y && iv.z == root.z) 
    //{
    //    connect_these(root, dir, hall);
    //    return true;
    //}
	return false;
}

IntVec3 root;
IntVec3 hall;
IntVec3 room;
bool empty_lat_space_around(IntVec3 iv) 
{
    // find out how many, and which are valid directions
    int num_choices = 0;
    direction_t choices[4]; // 4 lateral possibilities

    for (int i = 0; i < 4; i++) // consider all lateral directions
    {
        switch((direction_t)i) 
        {
            case DIR_NORTH: if (valid_room_idx_to_dir_from(DIR_NORTH, iv) && rooms[iv.z][iv.y + 1][iv.x].dead) choices[num_choices++] = DIR_NORTH; break;
            case DIR_SOUTH: if (valid_room_idx_to_dir_from(DIR_SOUTH, iv) && rooms[iv.z][iv.y - 1][iv.x].dead) choices[num_choices++] = DIR_SOUTH; break;
            case DIR_EAST:  if (valid_room_idx_to_dir_from(DIR_EAST,  iv) && rooms[iv.z][iv.y][iv.x + 1].dead) choices[num_choices++] = DIR_EAST; break;
            case DIR_WEST:  if (valid_room_idx_to_dir_from(DIR_WEST,  iv) && rooms[iv.z][iv.y][iv.x - 1].dead) choices[num_choices++] = DIR_WEST; break;

            case DIR_UP:
            case DIR_DOWN:
            case DIR_MAX:
                GS_ASSERT(false);
                break;
        }
    }

    if (num_choices < 1) return false;
    direction_t dir = choices[randrange(0, num_choices - 1)]; // get random dir
    
    if (/* root passed */ iv.x == root.x && iv.y == root.y && iv.z == root.z) 
    {
        connect_these(root, dir, hall);
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
        printf("Ruins generator: neither root or hall was passed into: bool empty_lat_space_around(IntVec3 iv)\n");
		GS_ASSERT(false);
        return false;
    }
}

void UNUSED_make_a_simple_room() 
{
    // spans refer to the AIRSPACE, and don't include outer shell of cubes
    // but offset, for cleaner comparisons, should actually be the absolute offset from the corner of the room (including shell)
    IntVec3 mal_span; // malleable/dynamic space span
    mal_span.x = CUBES_ACROSS_ROOM - 2 /* shell of 2 walls */ - MIN_LIP * 2;
    mal_span.y = CUBES_ACROSS_ROOM - 2 /* shell of 2 walls */ - MIN_LIP * 2;
    mal_span.z = CUBES_GOING_UP_ROOM    - 2 /* shell of 2 walls */ - MIN_LIP; // floors of the same height are fine, altho ceiling should be lipped

    // fixme    to provide stairs ONLY in rooms, and choose one AFTER a whole floor is generated
    int stairway_up_x = randrange(0, ROOMS_GOING_ACROSS - 1);
    int stairway_up_y = randrange(0, ROOMS_GOING_ACROSS - 1);



            //// connections in directions (inclu stairs)
            //for (int i = 0; i < DIR_MAX; i++) {
            //    if /* lateral dir */ (i < DIR_UP) 
            //        r.dir_types[i] = (direction_type_t)randrange(1, 2); // randomly choose door or hall
            //    else if /* stairway up should be here */ (i == DIR_UP && z < ROOMS_GOING_UP - 1 && x == stairway_up_x && y == stairway_up_y)
            //        r = setup_stairspace_for(DIR_UP, r);
            //    else {
            //        r.dir_types[i] = DIRTYPE_BLOCKED_FOREVER;
            //    }
            //}

            //if /* stairs going upwards in room below */ (z > 0 && rooms[z - 1][y][x].dir_types[DIR_UP] == DIRTYPE_STAIRS)
            //    r = setup_stairspace_for(DIR_DOWN, r);
}

void make_alive_and_setup(Room& r, room_t room_t) 
{
    //printf("make_alive_and_setup\n");

    r.dead = false;
	r.room_t = room_t;

    // for now we're randomizing each room for maximum debug patchwork quilting
	r.floo = randcube(floors, NUM_FLOOR_CUBES);//floo;
    r.wall = randcube(walls, NUM_WALL_CUBES);//wall;
    r.ceil = randcube(ceils, NUM_CEIL_CUBES);//ceil;
    r.trim = randcube(trims, NUM_TRIM_CUBES);//trim;

    // offset openings, to get inside the outermost layer of room space
    r.nh.z = 1;
    r.sh.z = 1;
    r.eh.z = 1;
    r.wh.z = 1;
    
	// always want to differ from default height
	r.nh.hei = HALLWAY_HEIGHT;
    r.sh.hei = HALLWAY_HEIGHT;
    r.eh.hei = HALLWAY_HEIGHT;
    r.wh.hei = HALLWAY_HEIGHT;

    // set subspace of grid node
	if (ROOMT_HALL == r.room_t) 
	{
		r.x = r.y = FIXED_CONNECTION_OFFSET;
		r.wid = r.dep = FIXED_CONNECTION_SPAN;
	}
	else // just 1 cube span inwards (of the room possibility space)
	{
		r.x = r.y = 1;
		r.wid = r.dep = CUBES_ACROSS_ROOM - 2;
	}
}

IntVec3 find_valid_root() 
{
    printf("find_valid_root() is doing NOTHING ATM!!!!!!***********\n");
        IntVec3 iv; // dead = true by default, and that can be checked, so that if it isn't an alive room returned, we know no valid roots could be found
    return iv;
}
    
void snake_a_new_path() 
{
    for (int z = 0; z < ROOMS_GOING_UP; z++) {
        printf("        floor == %d ----------------------------------------------------\n", z);

        // setup floorwide settings
        // this root room will be diff for each floor.  it's the corner of a boss room on floor 0 (which will be a 2x3 joining of grid nodes, prob should exceptionally generate bossroom the moment
                        // the corner room is established.  since it would be easy to special case, and we could skip its other nodes in the later generation stage
        root.x = randrange(0, ROOMS_GOING_ACROSS - 1 - 3 /* potential boss room wid */);
        root.y = randrange(0, ROOMS_GOING_ACROSS - 1 - 3 /* potential boss room dep */);
        root.z = z;
        make_alive_and_setup(rooms[root.z][root.y][root.x], ROOMT_NORMAL);
        
        CubeType floo = randcube(floors, NUM_FLOOR_CUBES);
        CubeType wall = randcube(walls, NUM_WALL_CUBES);
        CubeType ceil = randcube(ceils, NUM_CEIL_CUBES);
        CubeType trim = randcube(trims, NUM_TRIM_CUBES);

        // for each room desired, keep trying to make a valid hall-then-room train (2 car) of space spans
        int num_desired_rooms = 12;
        for (int i = 0; i < num_desired_rooms; i++) {

            int num_tries = 0;
            if (empty_lat_space_around(root))
            {
                num_tries++;
                if (num_tries > 4000) {printf(">4000 tries!  I GIVE!\n");break;}
        
                if (empty_lat_space_around(hall) )
                {
                    make_alive_and_setup(rooms[hall.z][hall.y][hall.x], ROOMT_HALL);
                    root.x = room.x;
                    root.y = room.y;
                    root.z = room.z;
                    make_alive_and_setup(rooms[room.z][room.y][room.x], ROOMT_NORMAL);
                }
            }
            else // must start a new snake, cuz can't build off ROOT room
            {
			    printf("else // must start a new snake, cuz can't build off ROOT room\n");

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

	snake_a_new_path();

    // generate each room
    for (int rx = 0; rx < ROOMS_GOING_ACROSS; rx++) {
    for (int ry = 0; ry < ROOMS_GOING_ACROSS; ry++) {
    for (int rz = 0; rz < ROOMS_GOING_UP; rz++) {
        if (rooms[rz][ry][rx].dead) continue;

        // make floor crust
        CubeType fl = (randrange(0,7) == 0) ? t_map::get_cube_type("rock") : rooms[rz][ry][rx].floo;
        t_gen::set_region(
            rx * CUBES_ACROSS_ROOM + x,
            ry * CUBES_ACROSS_ROOM + y,
            rz * CUBES_GOING_UP_ROOM + BEDROCK_OFFSET,
            CUBES_ACROSS_ROOM, CUBES_ACROSS_ROOM, 1, fl);

        // make ceiling crust
        //t_gen::set_region(
        //    rx * CUBES_ACROSS_ROOM + x,
        //    ry * CUBES_ACROSS_ROOM + y,
        //    rz * CUBES_GOING_UP_ROOM + BEDROCK_OFFSET + CUBES_GOING_UP_ROOM - 1,
        //    CUBES_ACROSS_ROOM, CUBES_ACROSS_ROOM, 1, rooms[rz][ry][rx].ceil);
        
        IntVec3 ri;  // room index
		ri.x = rx; 
		ri.y = ry; 
		ri.z = rz;
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

        if(rooms == NULL) 
        {
            rooms = new Room**[ROOMS_GOING_UP];
            for(int i=0; i<ROOMS_GOING_UP; i++)
            {
                rooms[i] = new Room*[ROOMS_GOING_ACROSS];
                for(int j=0; j<ROOMS_GOING_ACROSS; j++)
                {
                    rooms[i][j] = new Room[ROOMS_GOING_ACROSS];
                }
            }
        }
        //rooms = new (Room[ROOMS_GOING_UP][ROOMS_GOING_ACROSS][ROOMS_GOING_ACROSS]);

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
