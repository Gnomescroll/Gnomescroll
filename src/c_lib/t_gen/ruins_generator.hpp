// fixme: look for all " = "
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
const int CUBES_ACROSS_ROOM = 10;
const int CUBES_GOING_UP_ROOM = 8;
const int BEDROCK_OFFSET = 3;
const int MIN_LIP = 2; // minimum lip
const int CONN_HEIGHT = 3; // connections between rooms are represented by Rect3Ds
const int CONN_SPAN = 2;
const int CONN_OFFSET = 4;

CubeType curr_floo = EMPTY_CUBE;
CubeType curr_wall = EMPTY_CUBE;
CubeType curr_ceil = EMPTY_CUBE;
CubeType curr_trim = EMPTY_CUBE;

enum direction_t 
{
    DIR_NORTH, 
	DIR_SOUTH,
    DIR_EAST, 
	DIR_WEST,
    DIR_UP, 
	DIR_DOWN,
    DIR_MAX // use only as a max value when iterating thru directions
};
        
enum room_t 
{
    ROOMT_NORMAL,
    ROOMT_HALL,
    ROOMT_BOSS_CORNER,
    ROOMT_BOSS_NORMAL,
};
        
CubeType randcube(CubeType arr[], int num) 
{
    return arr[randrange(0, num - 1)];
}

class IntVec3 
{
    public :
        int x;
        int y;
        int z;

    IntVec3() { x = y = z = 0; }
    IntVec3(int x_, int y_, int z_) 
    { 
        x = x_;
        y = y_;
        z = z_; 
    }

    bool Equals(IntVec3 iv) 
    {
        if (x == iv.x && 
            y == iv.y && 
            z == iv.z
        ) 
            return true;

        return false;
    }

    void Clone(IntVec3 iv) 
    {
        x = iv.x;
        y = iv.y;
        z = iv.z;
    }
};
    
struct Rect 
{
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
    
struct Rect3D 
{
    int x, y, z, wid, dep, hei;

    Rect3D() 
    { 
        x = y = z = wid = dep = hei = 0; 
    }
    Rect3D(int x_, int y_, int z_, int wid_, int dep_, int hei_) 
    { 
        x = x_; 
        y = y_; 
        z = z_; 
        wid = wid_; 
        dep = dep_; 
        hei = hei_; 
    }

    void Clone(Rect3D r) 
    { 
        x = r.x;
        y = r.y;
        z = r.z;
        wid = r.wid;
        dep = r.dep;
        hei = r.hei;
    }

    void Close() 
    { 
        x = y = z = wid = dep = hei = 0; 
    }
};
    
Rect3D fixed_stair(3, 4, 0, 4, 2, 1);

struct Room 
{
	int id; // just used to label the order the rooms were generated
    bool dead;
    enum room_t room_t;
    CubeType wall;
    CubeType floo;
    CubeType ceil;
    CubeType trim;
	// connections in 6 directions
    Rect3D uconn;
    Rect3D dconn;
    Rect3D nconn;
    Rect3D sconn; 
    Rect3D econn;
    Rect3D wconn;
    Rect3D air;    // the clear/air region that connections are up against

    Room() 
	{
		id = 0;
        dead = true;
        room_t = ROOMT_NORMAL;
        wall = floo = ceil = trim = EMPTY_CUBE;
    }
};

//Room rooms[ROOMS_GOING_UP][ROOMS_GOING_ACROSS][ROOMS_GOING_ACROSS];
Room*** rooms = NULL; //[ROOMS_GOING_UP][ROOMS_GOING_ACROSS][ROOMS_GOING_ACROSS];

IntVec3 get_room_origin(IntVec3 ri /* room index */, int ox, int oy)
{
	IntVec3 iv;
	iv.x = ri.x * CUBES_ACROSS_ROOM + ox;  
	iv.y = ri.y * CUBES_ACROSS_ROOM + oy;  
	iv.z = ri.z * CUBES_GOING_UP_ROOM + BEDROCK_OFFSET; 
	return iv;
}

void set_region(int x_, int y_, int z_, int wid, int dep, int hei, CubeType ct)
{
    for (int z = z_; z < z_ + hei; z++) 
    for (int y = y_; y < y_ + dep; y++) 
    for (int x = x_; x < x_ + wid; x++) 
        t_map::set_fast(x, y, z, ct);
}
void set_region(Rect3D r, CubeType ct, IntVec3 ri /* room index */, int ox, int oy) // x/y offset
{
    IntVec3 ori = get_room_origin(ri, ox, oy);

    set_region(
		ori.x + r.x, 
		ori.y + r.y, 
		ori.z + r.z, 
		r.wid, r.dep, r.hei, 
		ct);
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

void setup_room(Room& r, room_t rt) 
{
    r.room_t = rt;

    //// for now we're randomizing each room for maximum debug patchwork quilting
    //r.floo = randcube(floors, NUM_FLOOR_CUBES);//floo;
    //r.wall = randcube(walls, NUM_WALL_CUBES);//wall;
    //r.ceil = randcube(ceils, NUM_CEIL_CUBES);//ceil;
    //r.trim = randcube(trims, NUM_TRIM_CUBES);//trim;

    // one up from omnipresent floor
    r.nconn.z = 1;
    r.sconn.z = 1;
    r.econn.z = 1;
    r.wconn.z = 1;
    r.air.z = 1;
    
    r.nconn.hei = CONN_HEIGHT;
    r.sconn.hei = CONN_HEIGHT;
    r.econn.hei = CONN_HEIGHT;
    r.wconn.hei = CONN_HEIGHT;
    r.air.hei = randrange(5, CUBES_GOING_UP_ROOM - 2);

    // set subspace of grid node
    if (ROOMT_HALL == r.room_t) 
    {
        r.air.x = r.air.y = CONN_OFFSET;
        r.air.wid = r.air.dep = CONN_SPAN;
        r.air.hei = CONN_HEIGHT;
    }
    else // just 1 cube span inwards (of the room possibility space)
    {
        r.air.x = r.air.y = 1;
        r.air.wid = r.air.dep = CUBES_ACROSS_ROOM - 2;
    }
}

void make_alive_and_setup(Room& r, room_t room_t) 
{
    r.dead = false;
    setup_room(r, room_t);
}

void make_chest(int x, int y, int z) 
{
    ItemContainerID id = ItemContainer::create_container_block(ItemContainer::name::storage_block_small, x, y, z);

	if (id != NULL_CONTAINER)
    {
        if (randrange(0, 2) == 0)
        {
            ItemContainer::auto_add_item_to_container("iron_bar", id);
            ItemContainer::auto_add_item_to_container("iridium_bar", id);
            ItemContainer::auto_add_item_to_container("gallium_bar", id);
            ItemContainer::auto_add_item_to_container("copper_bar", id);
        }
        if (randrange(0, 1) == 0)
        {
            ItemContainer::auto_add_item_to_container("coal_nugget", id);
        }
        if (randrange(0, 3) == 0)
        {
            ItemContainer::auto_add_item_to_container("green_mining_laser", id);
            ItemContainer::auto_add_item_to_container("laser_rifle", id);
        }
        if (randrange(0, 4) == 0)
        {
            ItemContainer::auto_add_item_to_container("small_charge_pack", id);
            ItemContainer::auto_add_item_to_container("energy_tank", id);
        }
    }
}

void set_at_room_offset(CubeType ct, int x, int y, int z, IntVec3 ri /* room index */, int ox, int oy)
{
    IntVec3 ori = get_room_origin(ri, ox, oy);
	t_map::set_fast(ori.x + x, ori.y + y, ori.z + z, ct);
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
        if (!rect_contains(r.air, cx, cy, cz) )
        {
            if (!rect_contains(r.uconn, cx, cy, cz)
                &&
                !rect_contains(r.dconn, cx, cy, cz)
                &&
                !rect_contains(r.nconn, cx, cy, cz)
                &&
                !rect_contains(r.sconn, cx, cy, cz)
                &&
                !rect_contains(r.econn, cx, cy, cz)
                &&
                !rect_contains(r.wconn, cx, cy, cz) 
            )
                need_cube = true;
        }

        // spawn cubes
        if (need_cube) 
        {
            // change rim/frame cubes
            if (r.room_t != ROOMT_HALL)
                if (cz == 1 ||
                    rect_plus_margin_contains(r.nconn, 1, cx, cy, cz) || 
                    rect_plus_margin_contains(r.sconn, 1, cx, cy, cz) || 
                    rect_plus_margin_contains(r.econn, 1, cx, cy, cz) || 
                    rect_plus_margin_contains(r.wconn, 1, cx, cy, cz) || 
                    rect_plus_margin_contains(r.uconn, 1, cx, cy, cz) || 
                    rect_plus_margin_contains(r.dconn, 1, cx, cy, cz)
                ) 
                    cube = r.trim;
            
			set_at_room_offset(cube,       cx, cy, cz, ri, ox, oy);
        } 
        else 
        {
			set_at_room_offset(EMPTY_CUBE, cx, cy, cz, ri, ox, oy);
            
            if (cz == 1 && (randrange(0, 156) == 0))
            { 
                make_chest(cx, cy, 64);//cz);
            }
        }

		// make column of cubes in the middle to show room id (should be the order rooms were generated)
		Rect3D r3d;
		r3d.z = 1;
		r3d.y = CUBES_ACROSS_ROOM / 2;
		r3d.x = CUBES_ACROSS_ROOM / 2;
		r3d.wid = r3d.dep = 1;
		r3d.hei = r.id;
		set_region(r3d, curr_floo, ri, ox, oy);
    }
}

void make_stairs(IntVec3 iv, int ox, int oy) 
{ // room indexes, origin
    CubeType floo = rooms[iv.z][iv.y][iv.x].floo;

    t_gen::set_region(
        iv.x * CUBES_ACROSS_ROOM + ox + fixed_stair.x,
        iv.y * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        iv.z * CUBES_GOING_UP_ROOM + 3 + 1,
        1, fixed_stair.dep, 2, floo);
    t_gen::set_region(
        iv.x * CUBES_ACROSS_ROOM + ox + fixed_stair.x + 1,
        iv.y * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        iv.z * CUBES_GOING_UP_ROOM + 3 + 2,
        1, fixed_stair.dep, 3, floo);
    t_gen::set_region(
        iv.x * CUBES_ACROSS_ROOM + ox + fixed_stair.x + 2,
        iv.y * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        iv.z * CUBES_GOING_UP_ROOM + 3 + 4,
        1, fixed_stair.dep, 3, floo);
    t_gen::set_region(
        iv.x * CUBES_ACROSS_ROOM + ox + fixed_stair.x + 3,
        iv.y * CUBES_ACROSS_ROOM + oy + fixed_stair.y,
        iv.z * CUBES_GOING_UP_ROOM + 3 + 6,
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

bool valid_room_idx_to_dir_from(direction_t dir, IntVec3 from) 
{ // room index
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
    setup_room(rm, rm.room_t); // at this point we don't know if both of the desired pair of adds were allocated

    switch(d)
    {
        case DIR_NORTH:
            rm.nconn.x = CONN_OFFSET;
            rm.nconn.y = rm.air.y + rm.air.dep;
            rm.nconn.wid = CONN_SPAN;
            rm.nconn.dep = CUBES_ACROSS_ROOM - rm.nconn.y;
            break;
        case DIR_SOUTH:
            rm.sconn.x = CONN_OFFSET;
            rm.sconn.y = 0;
            rm.sconn.wid = CONN_SPAN;
            rm.sconn.dep = CONN_OFFSET;  // rm.air.y;
            break;
        case DIR_EAST:
            rm.econn.x =  rm.air.x + rm.air.wid;
            rm.econn.y = CONN_OFFSET;
            rm.econn.wid = CUBES_ACROSS_ROOM - rm.econn.x;
            rm.econn.dep = CONN_SPAN;
            break;
        case DIR_WEST:
            rm.wconn.x =  0;
            rm.wconn.y = CONN_OFFSET;
            rm.wconn.wid = CONN_OFFSET;  // rm.air.x;
            rm.wconn.dep = CONN_SPAN;
            break;
        case DIR_UP:// FIXME  must close down these connections btween each dungeon
            rm.uconn.Clone(fixed_stair);
            rm.uconn.z =  CUBES_GOING_UP_ROOM - 1;
            break;
        case DIR_DOWN:
            rm.dconn.Clone(fixed_stair);
            break;

        case DIR_MAX:
            GS_ASSERT(false);
            break;
    }
}

void connect_these(IntVec3& src, direction_t d, IntVec3& dst) 
{
        dst.Clone(src);  // starting point, gets offset in correct direction below

        switch(d) 
        {
            case DIR_NORTH: 
                dst.y++; 
                open_connection_to(DIR_NORTH, rooms[src.z][src.y][src.x]);
                open_connection_to(DIR_SOUTH, rooms[dst.z][dst.y][dst.x]);
                break;
            case DIR_SOUTH: 
                dst.y--; 
                open_connection_to(DIR_SOUTH, rooms[src.z][src.y][src.x]);
                open_connection_to(DIR_NORTH, rooms[dst.z][dst.y][dst.x]);
                break;
            case DIR_EAST:  
                dst.x++; 
                open_connection_to(DIR_EAST, rooms[src.z][src.y][src.x]);
                open_connection_to(DIR_WEST, rooms[dst.z][dst.y][dst.x]);
                break;
            case DIR_WEST:  
                dst.x--; 
                open_connection_to(DIR_WEST, rooms[src.z][src.y][src.x]);
                open_connection_to(DIR_EAST, rooms[dst.z][dst.y][dst.x]);
                break;
            case DIR_UP:
                dst.z++; 
                open_connection_to(DIR_UP,   rooms[src.z][src.y][src.x]);
                open_connection_to(DIR_DOWN, rooms[dst.z][dst.y][dst.x]);
                break;

            case DIR_DOWN:
            case DIR_MAX:
                GS_ASSERT(false);
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
    
    if (iv.Equals(root)) 
    {
		setup_room(rooms[root.z][root.y][root.x], rooms[root.z][root.y][root.x].room_t);  // fixme?   room_t might not be deliberately set yet?
        setup_room(rooms[hall.z][hall.y][hall.x], ROOMT_HALL);
        connect_these(root, dir, hall);
        return true;
    }
    else 
    if (iv.Equals(hall)) 
    {
        setup_room(rooms[hall.z][hall.y][hall.x], ROOMT_HALL);
        setup_room(rooms[room.z][room.y][room.x], ROOMT_NORMAL);
        connect_these(hall, dir, room);
        return true;
    }
    else
    {   
        printf("Ruins generator: neither root or hall was passed into: bool empty_lat_space_around(iv)\n");
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
    //int stairway_up_x = randrange(0, ROOMS_GOING_ACROSS - 1);
    //int stairway_up_y = randrange(0, ROOMS_GOING_ACROSS - 1);



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

void finish_room(IntVec3 iv, int id = 0) 
{
    rooms[iv.z][iv.y][iv.x].wall = curr_wall;
    rooms[iv.z][iv.y][iv.x].floo = curr_floo;
    rooms[iv.z][iv.y][iv.x].ceil = curr_ceil;
    rooms[iv.z][iv.y][iv.x].trim = curr_trim;
    rooms[iv.z][iv.y][iv.x].dead = false;
    rooms[iv.z][iv.y][iv.x].id = id;
} 


IntVec3 find_valid_root() 
{
    IntVec3 iv;
	int num_alives = 0;

    // 1st pass: find out how many alives, and assure we have a dead value to return, if nothing valid
	for (int rx = 0; rx < ROOMS_GOING_ACROSS; rx++)
    for (int ry = 0; ry < ROOMS_GOING_ACROSS; ry++)
    for (int rz = 0; rz < ROOMS_GOING_UP; rz++) 
	{
        if (rooms[rz][ry][rx].dead)
		{
			iv.x = rx;
			iv.y = ry;
			iv.z = rz;
		}
		else
		{
			num_alives++;
		}
    }

	if (num_alives == 0)
		return iv; 

    // 2nd pass: return a random alive
	int chosen_alive = randrange(0, num_alives - 1);
	num_alives = 0;  // can reuse this now that we got random # using it
	for (int rx = 0; rx < ROOMS_GOING_ACROSS; rx++)
    for (int ry = 0; ry < ROOMS_GOING_ACROSS; ry++)
    for (int rz = 0; rz < ROOMS_GOING_UP; rz++) 
	{
        if (!rooms[rz][ry][rx].dead)
		{
			if (num_alives == chosen_alive)
			{
				iv.x = rx;
				iv.y = ry;
				iv.z = rz;
				return iv;
			}

			num_alives++;
		}
    }

    return iv;  // will be dead if we don't return somewhere earlier....
				// then we know no valid roots could be found
}

bool added_pair(int id)
{
    int num_tries = 0;
    if (empty_lat_space_around(root))
    {
        num_tries++;
        if (num_tries > 4000) 
		{
			printf("ruins_generator: > 4000 tries!  I GIVE!\n");
			return false;
		}
        
        if (empty_lat_space_around(hall) )
        {
            finish_room(hall, id);
            finish_room(room, id);
            root.Clone(room);
			return true;
        }
    }

	return false;
}

void set_pathing_data(int ox, int oy)  // origin x/y
{
    // setup ruinswide settings
    curr_floo = randcube(floors, NUM_FLOOR_CUBES);
    curr_wall = randcube(walls, NUM_WALL_CUBES);
    curr_ceil = randcube(ceils, NUM_CEIL_CUBES);
    curr_trim = randcube(trims, NUM_TRIM_CUBES);
        
    for (int z = 0; z < ROOMS_GOING_UP; z++) 
	{
        // make boss room or stairway-down
        if (z == 0) // (bottom level) 2x3 joining of grid nodes
        {
            root.x = randrange(0, ROOMS_GOING_ACROSS - 1 - 3 /* potential boss room wid */);
            root.y = randrange(0, ROOMS_GOING_ACROSS - 1 - 3 /* potential boss room dep */);
            root.z = z;
        }
        else  // need stairs
        {
            IntVec3 lower;
            lower.Clone(root); 
            root.z++;
            connect_these(lower, DIR_UP, root);
        }
        
        make_alive_and_setup(rooms[root.z][root.y][root.x], ROOMT_NORMAL);
        finish_room(root);
       
        // snake a linear path
        // for each pair desired, keep trying to make a valid hall-then-room (2 car) train of Rect3D spans
        for (int i = 0; i < 5; i++) 
		{
            if (!added_pair(i + 1))  // must start a new snake, cuz can't build off ROOT room
            {
                root = find_valid_root();
				if (rooms[root.z][root.y][root.x].dead) 
					break;  // couldn't find valid room
            }
        }

        // add offshoot pairs randomly
        // for each pair desired, keep trying to make a valid hall-then-room (2 car) train of Rect3D spans
        for (int i = 0; i < 7; i++) 
		{
            root = find_valid_root();
			if (rooms[root.z][root.y][root.x].dead) 
				break;  // couldn't find valid room

            if (!added_pair(i + 1 + 5 /* how many pairs probably made */))  // must start a new snake, cuz can't build off ROOT room
            {
				break;  // couldn't find valid room
            }
		}
    }
}
    
void draw_ASCII_floorplan(int level, int northernmost, int southernmost)
{
    printf("_____________________________ level %d ___________________________\n", level);

    for (int ry = northernmost; ry >= southernmost; ry--)
    {
        for (int rx = 0; rx < ROOMS_GOING_ACROSS; rx++) // do X innermostly to make ASCII easy
        {
            // show in ASCII
            if (rooms[level][ry][rx].dead) 
            {
                printf("  ");
                continue;
            }
            else
            {
                if (rooms[level][ry][rx].room_t == ROOMT_HALL)
                    printf("%c%c", 176, 176);
                else
                    printf("%c%c", 219, 219);
            }
        }

        printf("\n");
    }
}

bool contains_1_or_more_cubes(Rect3D r)
{
    int tot_spans = 0;

    if (r.wid > 0)
        tot_spans++;
    if (r.dep > 0)
        tot_spans++;
    if (r.hei > 0)
        tot_spans++;

    if (tot_spans > 1)
        return true;

    return false;
}

void make_ruins(int x, int y) {
    printf("__________________________________________________________________\n");
    printf("Making ruin @ %d, %d\n", x, y);

    set_pathing_data(x, y);

    // generate each room's cubes
    for (int rz = 0; rz < ROOMS_GOING_UP; rz++) 
    {
        int northernmost = 0;
        int southernmost = ROOMS_GOING_ACROSS;
        for (int rx = 0; rx < ROOMS_GOING_ACROSS; rx++)
        for (int ry = 0; ry < ROOMS_GOING_ACROSS; ry++)
        {
            if (rooms[rz][ry][rx].dead) 
                continue;

            if (northernmost < ry)
                northernmost = ry;
            if (southernmost > ry)
                southernmost = ry;

            // make floor crust
            CubeType fl = (randrange(0,7) == 0) ? t_map::get_cube_type("rock") : rooms[rz][ry][rx].floo;
            t_gen::set_region(
                rx * CUBES_ACROSS_ROOM + x,
                ry * CUBES_ACROSS_ROOM + y,
                rz * CUBES_GOING_UP_ROOM + BEDROCK_OFFSET,
                CUBES_ACROSS_ROOM, CUBES_ACROSS_ROOM, 1, fl);

            // make ceiling crust
            t_gen::set_region(
                rx * CUBES_ACROSS_ROOM + x,
                ry * CUBES_ACROSS_ROOM + y,
                rz * CUBES_GOING_UP_ROOM + BEDROCK_OFFSET + CUBES_GOING_UP_ROOM - 1,
                CUBES_ACROSS_ROOM, CUBES_ACROSS_ROOM, 1, rooms[rz][ry][rx].ceil);
        
            IntVec3 ri;  // room index
            ri.x = rx; 
            ri.y = ry; 
            ri.z = rz;
            make_room_filling(ri, x, y);

            if (contains_1_or_more_cubes(rooms[rz][ry][rx].uconn))
            {
                make_stairs(ri, x, y);
                set_region(rooms[rz][ry][rx].uconn, EMPTY_CUBE, ri, x, y);
            }
            if (contains_1_or_more_cubes(rooms[rz][ry][rx].dconn))
                set_region(rooms[rz][ry][rx].dconn, EMPTY_CUBE, ri, x, y);
        }

        draw_ASCII_floorplan(rz, northernmost, southernmost);
    }
}

    void check_textures(CubeType arr[], int num) {
        for (int i = 0; i < num; i++) 
		{ 
            GS_ASSERT(t_map::isValidCube(trims[i])); 

            if (!t_map::isValidCube(trims[i])) 
			{ 
				printf("** cube id %d invalid ***", trims[i]); return; 
			}
        }
    }

    void reset_to_dead_and_closed()
    {
        for (int rx = 0; rx < ROOMS_GOING_ACROSS; rx++)
        for (int ry = 0; ry < ROOMS_GOING_ACROSS; ry++)
        for (int rz = 0; rz < ROOMS_GOING_UP; rz++) 
		{
            rooms[rz][ry][rx].dead = true;
            rooms[rz][ry][rx].uconn.Close();
            rooms[rz][ry][rx].dconn.Close();
            rooms[rz][ry][rx].nconn.Close();
            rooms[rz][ry][rx].sconn.Close();
            rooms[rz][ry][rx].econn.Close();
            rooms[rz][ry][rx].wconn.Close();
            rooms[rz][ry][rx].wall = 
            rooms[rz][ry][rx].floo = 
            rooms[rz][ry][rx].ceil = 
            rooms[rz][ry][rx].trim = EMPTY_CUBE;

        }
    }

    void generate_ruins() {
        // setup rooms array
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

        // texture set lists
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

        // check that they're valid
		check_textures(floors, NUM_FLOOR_CUBES);
        check_textures(walls,  NUM_WALL_CUBES);
        check_textures(ceils,  NUM_CEIL_CUBES);
        check_textures(trims,  NUM_TRIM_CUBES);

        // generate all ruins
        for (int i = 0; i < 3; i++)
        {
            make_ruins
            (
                randrange(0, t_map::map_dim.x - 1), 
                randrange(0, t_map::map_dim.y - 1)
            );

            reset_to_dead_and_closed();
        }
    }
}   // t_gen
