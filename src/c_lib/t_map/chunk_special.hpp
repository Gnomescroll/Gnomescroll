#pragma once

#ifdef DC_SERVER
#include <t_map/server/subscription_list.hpp>
#include <t_map/server/manager.hpp>
#include <t_map/net/t_StoC.hpp>
#endif

#include <item/common/constants.hpp>
#include <item/common/enum.hpp>

namespace t_map
{


struct inventory_block
{
    int x,y,z;
    ItemContainerType container_type;
    int container_id;
};

class CHUNK_ITEM_CONTAINER
{
    private:
        void remove_index(int index);
        
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
        for (int i=0; i<ibam; i++)
        {
            iba[i].container_id = NULL_CONTAINER;
            iba[i].container_type = CONTAINER_TYPE_NONE;
        }
    }

    ~CHUNK_ITEM_CONTAINER()
    {
        free(iba);
    }


    #if DC_CLIENT
    //deletes all
    void _reset()
    {
        iban = 0;
        for (int i=0; i<ibam; i++)
        {
            iba[i].container_id = NULL_CONTAINER;
            iba[i].container_type = CONTAINER_TYPE_NONE;
        }
    }
    #endif

    void _remove(int index);

    void remove(int x, int y, int z);
    void remove(int container_id);

    void get_container_location(int container_id, int position[3])
    {
        for (int i=0; i<iban; i++)
            if (iba[i].container_id == container_id)
            {
                position[0] = iba[i].x;
                position[1] = iba[i].y;
                position[2] = iba[i].z;
                return;
            }

        printf("ERROR - container %d not found in chunk %d\n", container_id, chunk_index);
        GS_ASSERT(false);
    }
    
    void add(int x, int y, int z, ItemContainerType container_type, int container_id);

    int get(int x, int y, int z)
    {
        for(int i=0; i<iban; i++)
            if(x == iba[i].x && y == iba[i].y && z == iba[i].z)
                return iba[i].container_id;
        return NULL_CONTAINER;
    }

    #if DC_SERVER
    void send_chunk_item_containers(ClientID client_id);
    void send_reset_chunk_item_containers(ClientID client_id);
    #endif
};





}
