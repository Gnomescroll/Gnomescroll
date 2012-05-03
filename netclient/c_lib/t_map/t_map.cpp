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
    #include <t_map/server/map_chunk_history.hpp>
    //#include <t_map/net/t_StoC.hpp>

    #include <common/random.h>
    #include <physics/vec3.hpp>

    #include <particle/_interface.hpp>

    #include <item/_interface.hpp>

    #include <t_map/config/drop_table.hpp>
#endif

struct MapDimension map_dim = { 512,512,128 };

namespace t_map
{

Terrain_map* main_map;

void init_t_map()
{
    //printf("init_t_map() \n");
    static int init = 0;
    if(init != 0) 
    {
        printf("error: init_t_map called twice \n");
        return;
    }
    init = 1;
    
    init_t_properties();

    main_map = new Terrain_map(MAP_WIDTH, MAP_HEIGHT); //512 by 512 map

    #if DC_CLIENT
    init_client_compressors();
    init_t_vbo();
    init_textures();
    #endif

    #if DC_SERVER
    map_history = new Terrain_map_history(MAP_WIDTH, MAP_HEIGHT);
    #endif

}

#if DC_CLIENT
void init_for_draw()
{
    //printf("init for draw \n");
    init_cache();
    init_shaders();
}
#endif
    
void end_t_map()
{
    //mz_inflateEnd()

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
class Terrain_map* get_map()
{
    return main_map;
}

int apply_damage(int x, int y, int z, int dmg)
{
    #if DC_CLIENT
    Sound::block_took_damage(x,y,z,0,0,0);
    #endif
    return t_map::main_map->apply_damage(x,y,z,dmg);
}

#if DC_SERVER


// apply block damage & broadcast the update to client
void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action)
{
    int block_type;
    int res = t_map::main_map->apply_damage(x,y,z, dmg, &block_type);
    if (res != 0) return;
/*
    NOTE!!!

    should only send to clients who are subscribed to map chunk
*/


    map_history->send_block_action(x,y,z,res,action);
/*
    block_action_StoC msg;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.val = res;
    msg.action = action;
    msg.broadcast();
*/

    handle_block_drop(x,y,z, block_type);
/*
    const float mom = 2.0f;
    float p = randf();
    if (p < 0.3)
    {
        x = (float)x + 0.5f + randf()/3;
        y = (float)y + 0.5f + randf()/3;
        z = (float)z + 0.05f;
        int type = randrange(0,7);
        Item::create_item_particle(type, x, y, z, (randf()-0.5f)*mom, (randf()-0.5f)*mom, mom);
    }
*/
}
#endif

int get_highest_open_block(int x, int y, int n) {
    if (n < 1) {
        printf("WARNING: _get_highest_open_block :: called with n < 1\n");
        return -1;
    }
    if (n==1) return get_highest_solid_block(x,y) + 1;

    int open=n;
    int block;
    int i;

    for (i=ZMAX-1; i>=0; i--) {
        block = _get(x,y,i);
        if (!isSolid(block)) {
            open++;
        } else {
            if (open >= n) {
                return i+1;
            }
            open = 0;
        }
    }
    if (open >= n) return 0;
    return -1;
}

int get_highest_open_block(int x, int y) 
{
    return get_highest_open_block(x,y,1);
}

int get_highest_solid_block(int x, int y, int z)
{
    int i = z-1;
    for (; i>=0; i--)
        if (isSolid(_get(x,y,i)))
            break;
    return i;
}

int get_lowest_open_block(int x, int y, int n)
{
    if (n < 1) {
        printf("WARNING: _get_lowest_open_block :: called with n < 1\n");
        return -1;
    }

    int i;
    int block;
    int open=0;
    for (i=0; i<ZMAX; i++) {
        block = _get(x,y,i);
        if (isSolid(block)) {
            open = 0;
        } else {
            open++;
        }
        if (open >= n) return i-open+1;
    }

    return i;
}

int get_lowest_solid_block(int x, int y)
{
    int i;
    for (i=0; i < ZMAX; i++) {
        if (isSolid(_get(x,y,i))) {
            break;
        }
    }
    if (i >= ZMAX) i = -1;  // failure
    return i;
}

}   // t_map::
 

int _get(int x, int y, int z)
{
    return t_map::main_map->get_block(x,y,z);
}

void _set(int x, int y, int z, int value)
{
    t_map::main_map->set_block(x,y,z,value);
}

/*
    WTF
*/
#if DC_SERVER
void _set_broadcast(int x, int y, int z, int value) 
{
    t_map::block_StoC msg;
    int i = _get(x,y,z);
    if (i != value) 
    {
        _set(x,y,z, value);
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.val = value;
        msg.broadcast();
    }

}

void send_map_metadata(int client_id)
{
    class t_map::map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.sendToClient(client_id);
}
#endif

/*
void send_map_metadata()
{
    class map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.broadcast();
}
*/

/*
void set_map_size(int x, int y, int z)
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
    send_map_metadata();
}
*/


/*
    Move this somewhere
*/

int get_height_at(int x, int y)
{
    for (int i=map_dim.z-1; i>=0; i--)
    {
        if (isSolid(_get(x,y,i)))
            return i;
    }
    return 0;
}

#if DC_CLIENT
unsigned char get_cached_height(int x, int y)
{
    if (t_map::main_map != NULL)
        return t_map::main_map->get_cached_height(x,y);
    return 0;
}
#endif

int _get_highest_open_block(int x, int y, int n)
{
    return t_map::get_highest_open_block(x,y,n);
}

int _get_highest_open_block(int x, int y) 
{
    return t_map::get_highest_open_block(x,y,1);
}

int _get_highest_solid_block(int x, int y, int z)
{
    return t_map::get_highest_solid_block(x,y,z);
}

int _get_lowest_open_block(int x, int y, int n)
{
    return t_map::get_lowest_open_block(x,y,n);
}

int _get_lowest_solid_block(int x, int y)
{
    return t_map::get_lowest_solid_block(x,y);
}

inline bool point_in_map(int x, int y, int z)
{
    if (x<0 || x>=map_dim.x || y<0 || y>=map_dim.y || z<0 || z>map_dim.z)
        return false;
    return true;
}

