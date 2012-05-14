#pragma once

#ifdef DC_SERVER
#include <t_map/server/subscription_list.hpp>
#include <t_map/server/manager.hpp>
#include <t_map/net/t_StoC.hpp>
#include <item/common/constant.hpp>
#endif

#include <item/common/constant.hpp>
#include <item/common/enum.hpp>

namespace t_map
{


struct inventory_block
{
    int x,y,z;
    int container_type;
    int container_id;
};

class CHUNK_ITEM_CONTAINER
{
    public:

    int chunk_index;

    int iban; //inventory_block_array_num
    int ibam; //inventory_block_array_max
    struct inventory_block* iba; //inventory_block_array

    CHUNK_ITEM_CONTAINER()
    {
        iban = 0;
        ibam = 2;
        iba = (struct inventory_block*) malloc(ibam*sizeof(struct inventory_block));
    }

    //deletes all
    void _reset()
    {
        iban = 0;
    }

    void _remove(int index)
    {
        GS_ASSERT(index < ibam);
        iban--;
        iba[index] = iba[iban];
    }

    void remove (int x, int y, int z)
    {
        int i;
        for(i=0; i<iban; i++)
        {
            if(x == iba[i].x && y == iba[i].y && z == iba[i].z)
            {
                break;
            }
        }
        if(i == iban) GS_ABORT();

        #if DC_SERVER
        map_history->container_block_delete(chunk_index, iba[i].container_id);
        #endif
        _remove(i);
    }

	void remove (int container_id)
	{
		int i;
		for(i=0; i<iban; i++)
		{
			if(container_id == iba[i].container_id)
			{
				break;
			}
		}
		if(i == iban) GS_ABORT();

		#if DC_SERVER
		map_history->container_block_delete(chunk_index, iba[i].container_id);
		#endif
		_remove(i);
		printf("inventory_block removed: handle items and inventory destruction \n");
	}


    void add(int x, int y, int z, int container_type, int container_id)
    {
        if(iban == ibam)
        {
            ibam *= 2;
            iba = (struct inventory_block*) realloc(iba, ibam*sizeof(struct inventory_block));
            if(iba == NULL) GS_ABORT();
        }

        iba[iban].x = x;
        iba[iban].y = y;
        iba[iban].z = z;
        iba[iban].container_type = container_type;
        iba[iban].container_id = container_id;
        iban++;

        #ifdef DC_SERVER
        map_history->container_block_create(chunk_index, x, y, z, container_type, container_id);
        #endif

    }

    int get(int x, int y, int z)
    {
        int i;
        for(i=0; i<iban; i++)
        {
            if(x == iba[i].x && y == iba[i].y && z == iba[i].z)
            {
                break;
            }
        }
        if(i==iban)
        {
            printf("CHUNK_ITEM_CONTAINER::get error, no container for %i %i %i \n", x,y,z);
            return NULL_CONTAINER;
        }
        else return iba[i].container_id;
    }

#if DC_SERVER
    void send_chunk_item_containers(int client_id)
    {
        {
            class container_block_chunk_reset_StoC msgr;
            msgr.chunk_index = chunk_index;
            msgr.sendToClient(client_id);
            //printf("chunk index scic = %i \n", chunk_index);
        }

        class container_block_create_StoC msg;

        for(int i=0; i < iban; i++)
        {
            msg.x = iba[i].x;
            msg.y = iba[i].y;
            msg.z = iba[i].z;
            msg.container_type = iba[i].container_type;
            msg.container_type = iba[i].container_id;
            msg.sendToClient(client_id);
        }
    }


    void send_reset_chunk_item_containers(int client_id)
    {
        class container_block_chunk_reset_StoC msg;
        msg.chunk_index = chunk_index;
        msg.sendToClient(client_id);
    }
#endif
};





}
