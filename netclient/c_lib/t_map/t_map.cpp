#include "t_map.hpp"

#include "t_vbo.hpp"
#include "t_map_class.hpp"

#ifdef DC_CLIENT
    #include "cache.hpp"
#endif

struct MapDimension map_dim = { 512,512,128 };

namespace t_map
{

Terrain_map* main_map;

void init_t_map()
{
    main_map = new Terrain_map(512, 512); //512 by 512 map
}

#ifdef DC_CLIENT
    void init_for_draw()
    {
        init_cache();
        init_t_vbo();
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


/*
    Move to utility functions
*/
int _get_highest_open_block(int x, int y)
{
    return 127;
}

int _get_highest_open_block(int x, int y, int agent_height)
{
    return 127;
}


#include "net_StoC.hpp"
// apply block damage & broadcast the update to client
int _apply_damage_broadcast(int x, int y, int z, int dmg)
{
    int res = _apply_damage(x,y,z, dmg);
    if (res == 0) {
        _block_broadcast(x,y,z, 0);
    }
    return res;
}


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

void send_map_metadata(int client_id)
{
    class map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.sendToClient(client_id);
}

void send_map_metadata()
{
    class map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.broadcast();
}

void set_map_size(int x, int y, int z)
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
    send_map_metadata();
}
