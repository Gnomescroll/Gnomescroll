#include "t_map.hpp"

#include "t_vbo.hpp"
#include "t_properties.hpp"

#include "t_map_class.hpp"

#ifdef DC_CLIENT
    #include <c_lib/t_map/glsl/cache.hpp>
    #include <c_lib/t_map/glsl/texture.hpp>
    #include <c_lib/t_map/glsl/shader.hpp>
#endif

#ifdef DC_SERVER
    #include <c_lib/t_map/server/manager.hpp>
    #include <c_lib/t_map/server/map_chunk_history.hpp>
    #include <c_lib/t_map/net/t_StoC.hpp>
#endif

struct MapDimension map_dim = { 512,512,128 };

namespace t_map
{

Terrain_map* main_map;

void init_t_map()
{

    static int init = 0;
    if(init != 0) 
    {
        printf("error: init_t_map called twice \n");
        return;
    }
    init = 1;
    
    init_t_properties();

    main_map = new Terrain_map(MAP_WIDTH, MAP_HEIGHT); //512 by 512 map
#ifdef DC_SERVER
    map_history = new Terrain_map_history(MAP_WIDTH, MAP_HEIGHT);
#endif
    //set(2,4,5, 10);
    //printf("get= %i \n", get(2,4,5) );
/*
#ifdef DC_SERVER
    for( int i=0; i<512; i++ )
    for( int j=0; j<512; j++ )
    {
        set(i,j, 1, 2);
        if(i%5 == 0)  set(i,j, 6, 2);
        if(j%7 == 0)  set(i,j, 7, 2);
    }
#endif
*/
}

class Terrain_map* get_map()
{
    return main_map;
}

#ifdef DC_CLIENT
    void init_for_draw()
    {
        printf("init for draw \n");

        init_cache();
        init_t_vbo();
        init_textures();
        init_shaders();
    }
#endif

int get(int x, int y, int z)
{
    return main_map->get_block(x,y,z);
}

void set(int x, int y, int z, int value)
{
    main_map->set_block(x,y,z,value);
}

int apply_damage(int x, int y, int z, int dmg)
{
    #ifdef DC_CLIENT
    Sound::block_took_damage(x,y,z,0,0,0);
    #endif
    return t_map::main_map->apply_damage(x,y,z,dmg);
}

#ifdef DC_SERVER
// apply block damage & broadcast the update to client
void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action)
{
    int res = apply_damage(x,y,z, dmg);
    if (res == 0)
    {
        block_action_StoC msg;
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.val = res;
        msg.action = action;
        msg.broadcast();
    }
}
#endif

}
 

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
#ifdef DC_SERVER
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

int _get_highest_open_block(int x, int y, int n) {
    if (n < 1) {
        printf("WARNING: _get_highest_open_block :: called with n < 1\n");
        return -1;
    }
    if (n==1) return _get_highest_solid_block(x,y) + 1;

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

int _get_highest_open_block(int x, int y) 
{
    return _get_highest_open_block(x,y,1);
}

int _get_highest_solid_block(int x, int y) {

    int i;
    for (i=ZMAX-1; i>=0; i--) {
        if (isSolid(_get(x,y,i))) {
            break;
        }
    }
    return  i;
}

int _get_lowest_open_block(int x, int y, int n) {
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

int _get_lowest_solid_block(int x, int y) {

    int i;
    for (i=0; i < ZMAX; i++) {
        if (isSolid(_get(x,y,i))) {
            break;
        }
    }
    if (i >= ZMAX) i = -1;  // failure
    return i;
}

inline bool point_in_map(int x, int y, int z)
{
    if (x<0 || x>=map_dim.x || y<0 || y>=map_dim.y || z<0 || z>map_dim.z)
        return false;
    return true;
}

