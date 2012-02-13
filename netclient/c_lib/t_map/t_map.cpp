#include "t_map.hpp"

#include "t_vbo.hpp"
#include "t_map_class.hpp"

#ifdef DC_CLIENT
    #include "cache.hpp"
    #include "texture.hpp"
    #include "shader.hpp"
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
    
    main_map = new Terrain_map(MAP_WIDTH, MAP_HEIGHT); //512 by 512 map

    //set(2,4,5, 10);
    //printf("get= %i \n", get(2,4,5) );

    for( int i=0; i<512; i++ )
    for( int j=0; j<512; j++ )
    {
        set(i,j, 1, 2);
        if(i%5 == 0)  set(i,j, 6, 2);
        if(j%7 == 0)  set(i,j, 7, 2);

    }

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

}
 

int _get(int x, int y, int z)
{
    return t_map::main_map->get_block(x,y,z);
}

void _set(int x, int y, int z, int value)
{
    t_map::main_map->set_block(x,y,z,value);
}

int _apply_damage(int x, int y, int z, int dmg)
{
    return t_map::main_map->apply_damage(x,y,z,dmg);
}

#include "net_StoC.hpp"
// apply block damage & broadcast the update to client
int _apply_damage_broadcast(int x, int y, int z, int dmg)
{
    int res = _apply_damage(x,y,z, dmg);

    printf("warning: _apply_damage_broadcast \n");
    //if (res == 0) {
    //    _block_broadcast(x,y,z, 0);
    //}
    return res;
}

/*
void _set_broadcast(int x, int y, int z, int value) 
{
    class block_StoC msg;
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

void _block_broadcast(int x, int y, int z, int value) 
{
    class block_StoC msg;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.val = value;
    msg.broadcast();
}
*/


void send_map_metadata(int client_id)
{
    class map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.sendToClient(client_id);
}

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