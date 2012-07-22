#include "t_map.hpp"

#include <t_map/_interface.hpp>

#include "t_properties.hpp"

#include "t_map_class.hpp"

#if DC_CLIENT
    #include <t_map/glsl/cache.hpp>
    #include <t_map/glsl/texture.hpp>
    #include <t_map/glsl/shader.hpp>

    #include <t_map/net/t_StoC.hpp>
#endif

#if DC_SERVER
    #include <t_map/server/manager.hpp>
    #include <t_map/server/subscription_list.hpp>

    #include <common/random.h>

    #include <particle/_interface.hpp>

    #include <item/_interface.hpp>

    #include <t_map/config/drop_table.hpp>
    #include <entity/object/main.hpp>
#endif

struct MapDimension map_dim = { 512,512,128 };

namespace t_map
{

class Terrain_map* main_map;

int get(int x, int y, int z)
{
    if((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return 0;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    class MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if(c == NULL) return 0;
    return c->e[ (z<<8)+((y&15)<<4)+(x&15) ].block;

}

void set(int x, int y, int z, int value)
{
    #if DC_SERVER
    if (value != get(x,y,z))
		t_map::destroy_item_container_block(x,y,z);
    #endif
    main_map->set_block(x,y,z,value);
}

inline void set_fast(int x, int y, int z, int value)
{
	main_map->set_block(x,y,z,value);
}

struct MAP_ELEMENT get_element(int x, int y, int z)
{
    if((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return NULL_MAP_ELEMENT;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    class MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if(c == NULL) return NULL_MAP_ELEMENT;
    return c->e[ (z<<8)+((y&15)<<4)+(x&15) ];
}

void set_palette(int x, int y, int z, int value)
{
    struct MAP_ELEMENT element = main_map->get_element(x,y,z);
    element.palette = value;
    main_map->set_element(x,y,z,element);
}


class Terrain_map* get_map()
{
    return main_map;
}

void init_t_map()
{   
    init_t_properties();

    main_map = new Terrain_map(MAP_WIDTH, MAP_HEIGHT); //512 by 512 map

    #if DC_CLIENT
    init_client_compressors();
    init_t_vbo();
    init_textures();
    #endif

    #if DC_SERVER
    map_history = new Terrain_map_subscription(MAP_WIDTH, MAP_HEIGHT);
    #endif
}

#if DC_CLIENT
void init_for_draw()
{
    init_cache();
    init_shaders();
}
#endif
    
void end_t_map()
{
    end_t_properties();
    delete main_map;

    #if DC_CLIENT
    end_client_compressors();
    end_t_vbo();
    #endif

    #if DC_SERVER
    delete map_history;
    #endif
}


int apply_damage(int x, int y, int z, int dmg)
{
    GS_ASSERT(dmg > 0);
    #if DC_CLIENT
    print_trace();
    Sound::block_took_damage(x+0.5f,y+0.5f,z+0.5f,0,0,0);
    #endif
    return t_map::main_map->apply_damage(x,y,z,dmg);
}

#if DC_SERVER
// apply block damage & broadcast the update to client
void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action)
{
    GS_ASSERT(dmg > 0);
    if (dmg <= 0) return;
    GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
    if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

    int block_type;
    int res = t_map::main_map->apply_damage(x,y,z, dmg, &block_type);
    if (res != 0) return;

    //NOTE: only sends to clients who are subscribed to map chunk

    map_history->send_block_action(x,y,z,res,action);

    if(cube_list[block_type].item_drop == true) 
        handle_block_drop(x,y,z, block_type);
}

void broadcast_set_block_action(int x, int y, int z, int block, int action)
{
    GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
    if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

    map_history->send_block_action(x,y,z,block,action);
}

void broadcast_set_block(int x, int y, int z, int block)
{
    GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
    if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

    main_map->set_block(x,y,z,block);
    map_history->send_set_block(x,y,z,block);
}

void broadcast_set_block_palette(int x, int y, int z, int block, int palette)
{
    GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
    if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

    struct MAP_ELEMENT e = {{{0}}};
    e.block = block;
    e.palette = palette;

    main_map->set_element(x,y,z,e);
    map_history->send_set_block_palette(x,y,z,block,palette);
}

#endif

inline int get_highest_open_block(int x, int y, int n)
{
    GS_ASSERT(n >= 1);
    if (n < 1) return -1;
    if (n == 1) return get_highest_open_block(x,y);

    int open=n;
    int block;
    int i;

    for (i=ZMAX-1; i>=0; i--)
    {
        block = get(x,y,i);
        if (!isSolid(block))
            open++;
        else
        {
            if (open >= n) return i+1;
            open = 0;
        }
    }
    if (open >= n) return 0;
    return -1;
}

inline int get_highest_open_block(int x, int y) 
{
    return get_highest_solid_block(x,y)+1;
}

inline int get_nearest_open_block(int x, int y, int z, int n)
{
    GS_ASSERT(n >= 1);
    if (n < 1) return z;

    int inc = 1;    // direction (+/- up/down)
    int up = z;     // up cursor
    int down = z-1; // down cursor
    while (down >= 0 && up < map_dim.z)
    {
        // choose start pt
        int start = up;
        if (inc < 0)
            start = down;
            
        // look for gap of height n
        bool found = true;
        for (int j=0; j<n; j++)
        {
            if (isSolid(x,y,start+j))
            {
                found = false;
                break;
            }
        }
        if (found) return start;
        
        // advance up/down cursors
        if (inc > 0)
            up++;
        else
            down--;
        
        // set direction
        if (up >= map_dim.z)
            inc = -1;
        else if (down < 0)
            inc = 1;
        else
            inc *= -1;  // flip
    }
    
    return -1;
}

inline int get_nearest_open_block(int x, int y, int z)
{
    return get_nearest_open_block(x,y,z,1);
}


inline int get_highest_solid_block(int x, int y)
{
    return get_highest_solid_block(x,y, map_dim.z);
}

inline int get_highest_solid_block(int x, int y, int z)
{
    #if DC_CLIENT
    return main_map->get_cached_height(x,y)-1;
    #endif

    #if DC_SERVER
    int i = z-1;
    for (; i>=0; i--)
        if (isSolid(x,y,i)) break;
    return i;
    #endif
}

inline int get_lowest_open_block(int x, int y, int n)
{
    GS_ASSERT(n > 0);
    if (n < 1) return -1;

    int i;
    int block;
    int open=0;
    for (i=0; i<ZMAX; i++)
    {
        block = get(x,y,i);
        if (isSolid(block)) open = 0;
        else open++;
        if (open >= n) return i-open+1;
    }

    return i;
}

inline int get_lowest_solid_block(int x, int y)
{
    int i;
    for (i=0; i < ZMAX; i++)
        if (isSolid(x,y,i)) break;
    if (i >= ZMAX) i = -1;  // failure
    return i;
}

inline bool position_is_loaded(int x, int y)
{
	#if DC_SERVER
	return true;
	#endif
	
	#if DC_CLIENT
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
	int cx = x / TERRAIN_CHUNK_WIDTH;
	int cy = y / TERRAIN_CHUNK_WIDTH;
	int chunk = cx + cy*MAP_CHUNK_XDIM;
	return (main_map->chunk_heights_status[chunk] != CHUNK_HEIGHT_UNSET);
	#endif
}

bool block_can_be_placed(int x, int y, int z, int value)
{   
    GS_ASSERT(value != 0);
    if (get(x,y,z) != 0) return false;
    // check against all spawners
    if (Objects::point_occupied_by_type(OBJECT_AGENT_SPAWNER, x,y,z))
        return false;
    return true;
}


}   // t_map
