#pragma once

#include <t_map/_interface.hpp>
#include <t_map/common/map_element.hpp>
#include <t_map/net/t_StoC.hpp>
#include <t_map/t_map.hpp>
#include <physics/quadrant.hpp>

namespace t_map
{

const int SMC_MAX_SUBSCRIBERS = HARD_MAX_CONNECTIONS * 2;

class MAP_CHUNK_SUBSCRIPTION
{
    public:

    int chunk_index;

    int version;

    unsigned short subscribers[ SMC_MAX_SUBSCRIBERS ];
    unsigned short chunk_aliases[ SMC_MAX_SUBSCRIBERS ]; 
    
    int subscriber_num;

    MAP_CHUNK_SUBSCRIPTION()
    {
        version = 1;    //chunk version number

        subscriber_num = 0;
    }

    ~MAP_CHUNK_SUBSCRIPTION()
    {
                
    }

    void add_subscriber(ClientID client_id, int chunk_alias, int client_map_version)
    {
        //check that they are not already subscribed
        for(int i=0; i < subscriber_num; i++) GS_ASSERT(subscribers[i] != client_id)

        subscribers[subscriber_num] = (unsigned short)client_id;
        chunk_aliases[subscriber_num] = (unsigned short)chunk_alias;
        subscriber_num++;
    }

    void remove_subscriber(ClientID client_id)
    {
        int i = 0;
        while(i < subscriber_num && subscribers[i] != client_id) i++;
        GS_ASSERT(i != subscriber_num)

        subscriber_num--;
        subscribers[i] = subscribers[subscriber_num];
        chunk_aliases[i] = chunk_aliases[subscriber_num];
    }

    void send_block_action(int x, int y, int z, CubeType cube_type, int action)
    {
        GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
        if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;
        
        block_action_StoC msg;
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.cube_type = cube_type;
        msg.action = action;

        for(int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }

    void send_set_block(int x, int y, int z, CubeType cube_type)
    {
        GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
        if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

        block_set_StoC msg;
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.cube_type = cube_type;

        for(int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }

    void send_set_block_palette(int x, int y, int z, CubeType cube_type, int palette)
    {
        GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
        if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

        block_set_palette_StoC msg;
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.cube_type = cube_type;
        msg.palette = palette;

        for(int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }

    void container_block_create(int x, int y, int z, int container_type, ItemContainerID container_id)
    {
        GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
        if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

        class container_block_create_StoC msg;
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.container_type = container_type;
        msg.container_id = container_id;

        for(int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }

    void container_block_delete(int chunk_index, ItemContainerID container_id)
    {
        class container_block_delete_StoC msg;
        msg.chunk_index = chunk_index;
        msg.container_id = container_id;

        for(int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }
};


class Terrain_map_subscription
{
    public:
    
    int xdim;
    int ydim;
    
    int xchunk_dim;
    int ychunk_dim;
    
    class MAP_CHUNK_SUBSCRIPTION* chunk;

    Terrain_map_subscription(int _xdim, int _ydim)
    {
        xdim = (_xdim/16)*16; 
        ydim = (_ydim/16)*16;
        GS_ASSERT(xdim == _xdim && ydim == _ydim);

        xchunk_dim = _xdim/16; 
        ychunk_dim = _ydim/16;

        chunk = new MAP_CHUNK_SUBSCRIPTION[xchunk_dim*ychunk_dim];

        for(int x=0; x < xchunk_dim; x++)
            for(int y=0; y < ychunk_dim; y++)
                chunk[y*xchunk_dim+x].chunk_index = y*xchunk_dim+x;
    }

    ~Terrain_map_subscription()
    {
        delete[] chunk;
    }

    void send_block_action(int x, int y, int z, CubeType value, int action)
    {
        GS_ASSERT(x >= 0 && x < xdim && y >= 0 && y < ydim);   //take this out eventually

        int _x = x/16;
        int _y = y/16;

        chunk[xchunk_dim*_y + _x].send_block_action(x,y,z,value,action);
    }

    void send_set_block(int x, int y, int z, CubeType cube_type)
    {
        GS_ASSERT(x >= 0 && x < xdim && y >= 0 && y < ydim);   //take this out eventually

        int _x = x/16;
        int _y = y/16;

        chunk[xchunk_dim*_y + _x].send_set_block(x,y,z,cube_type);
    }

    void send_set_block_palette(int x, int y, int z, CubeType cube_type, int palette)
    {
        GS_ASSERT(x >= 0 && x < xdim && y >= 0 && y < ydim);   //take this out eventually

        int _x = x/16;
        int _y = y/16;

        chunk[xchunk_dim*_y + _x].send_set_block_palette(x,y,z,cube_type,palette);
    }

    void container_block_create(int chunk_index, int x, int y, int z, int container_type, ItemContainerID container_id)
    {
        chunk[chunk_index].container_block_create(x,y,z,container_type,container_id);
    }

    void container_block_delete(int chunk_index, ItemContainerID container_id)
    {
        chunk[chunk_index].container_block_delete(chunk_index, container_id);
    }

    unsigned short* get_subscribers(int chunk_index, int* subscriber_count)
    {
        *subscriber_count = chunk[chunk_index].subscriber_num;
        return chunk[chunk_index].subscribers;
    }

};

}   // t_map
