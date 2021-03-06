/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <t_map/_interface.hpp>
#include <t_map/common/map_element.hpp>
#include <t_map/net/t_StoC.hpp>
#include <t_map/t_map.hpp>
#include <physics/quadrant.hpp>

namespace t_map
{

const int SMC_MAX_SUBSCRIBERS = HARD_MAX_CONNECTIONS * 2;

class MapChunkSubscription
{
    public:

        int chunk_index;

        int version;    // chunk version number

        unsigned short subscribers[SMC_MAX_SUBSCRIBERS];
        unsigned short chunk_aliases[SMC_MAX_SUBSCRIBERS];

        int subscriber_num;

    MapChunkSubscription() :
        version(1), subscriber_num(0)
    {
    }

    ~MapChunkSubscription()
    {
    }

    void add_subscriber(ClientID client_id, int chunk_alias, int client_map_version)
    {
        //check that they are not already subscribed
        for (int i=0; i < subscriber_num; i++) GS_ASSERT(subscribers[i] != client_id)

        subscribers[subscriber_num] = (unsigned short)client_id;
        chunk_aliases[subscriber_num] = (unsigned short)chunk_alias;
        subscriber_num++;
    }

    void remove_subscriber(ClientID client_id)
    {
        int i = 0;
        while (i < subscriber_num && subscribers[i] != client_id) i++;
        GS_ASSERT(i != subscriber_num)

        subscriber_num--;
        subscribers[i] = subscribers[subscriber_num];
        chunk_aliases[i] = chunk_aliases[subscriber_num];
    }

    void send_block_action(const Vec3i& position, CubeType cube_type, int action)
    {
        IF_ASSERT(!is_boxed_position(position)) return;

        block_action_StoC msg;
        msg.position = position;
        msg.cube_type = cube_type;
        msg.action = action;

        for (int i=0; i < subscriber_num; i++)
            msg.sendToClient((ClientID)subscribers[i]);
    }

    void send_set_block(const Vec3i& position, CubeType cube_type)
    {
        IF_ASSERT(!is_boxed_position(position)) return;

        block_set_StoC msg;
        msg.position = position;
        msg.cube_type = cube_type;

        //printf("send_set_block: cube_type= %d, x,y,z= %d %d %d \n", cube_type, x,y,z);

        for (int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }

    void send_set_block_palette(const Vec3i& position, CubeType cube_type, int palette)
    {
        IF_ASSERT(!is_boxed_position(position)) return;
        block_set_palette_StoC msg;
        msg.position = position;
        msg.cube_type = cube_type;
        msg.palette = palette;

        for (int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }

    void container_block_create(const Vec3i& position, int container_type, ItemContainerID container_id)
    {
        IF_ASSERT(!is_boxed_position(position)) return;
        class container_block_create_StoC msg;
        msg.position = position;
        msg.container_type = container_type;
        msg.container_id = container_id;

        for (int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }

    void container_block_delete(int chunk_index, ItemContainerID container_id)
    {
        class container_block_delete_StoC msg;
        msg.chunk_index = chunk_index;
        msg.container_id = container_id;

        for (int i=0; i < subscriber_num; i++)
        {
            msg.sendToClient((ClientID)subscribers[i]);
        }
    }
};


class TerrainMapSubscription
{
    public:

        int xdim;
        int ydim;

        int xchunk_dim;
        int ychunk_dim;

        class MapChunkSubscription* chunk;

    TerrainMapSubscription(int _xdim, int _ydim)
    {
        xdim = (_xdim/16)*16;
        ydim = (_ydim/16)*16;
        GS_ASSERT(xdim == _xdim && ydim == _ydim);

        xchunk_dim = _xdim/16;
        ychunk_dim = _ydim/16;

        chunk = new MapChunkSubscription[xchunk_dim*ychunk_dim];

        for (int x=0; x < xchunk_dim; x++)
            for (int y=0; y < ychunk_dim; y++)
                chunk[y*xchunk_dim+x].chunk_index = y*xchunk_dim+x;
    }

    ~TerrainMapSubscription()
    {
        delete[] chunk;
    }

    void send_block_action(const Vec3i& position, CubeType value, int action)
    {
        IF_ASSERT(!is_boxed_position(position)) return;
        int cx = position.x/16;
        int cy = position.y/16;
        chunk[xchunk_dim*cy + cx].send_block_action(position, value, action);
    }

    void send_set_block(const Vec3i& position, CubeType cube_type)
    {
        IF_ASSERT(!is_boxed_position(position)) return;
        int x = position.x / 16;
        int y = position.y / 16;
        chunk[xchunk_dim*y + x].send_set_block(position, cube_type);
    }

    void send_set_block_palette(const Vec3i& position, CubeType cube_type, int palette)
    {
        IF_ASSERT(!is_boxed_position(position)) return;
        int x = position.x / 16;
        int y = position.y / 16;
        chunk[xchunk_dim*y + x].send_set_block_palette(position, cube_type, palette);
    }

    void container_block_create(int chunk_index, const Vec3i& position, int container_type, ItemContainerID container_id)
    {
        chunk[chunk_index].container_block_create(position, container_type, container_id);
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
