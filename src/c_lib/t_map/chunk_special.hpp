#pragma once

#if DC_SERVER
#include <t_map/server/subscription_list.hpp>
#include <t_map/server/manager.hpp>
#include <t_map/net/t_StoC.hpp>
#endif

#include <item/common/constants.hpp>
#include <item/common/enum.hpp>

namespace t_map
{

struct InventoryBlock
{
    int x,y,z;
    ItemContainerType container_type;
    ItemContainerID container_id;
};

class ChunkItemContainer
{
    private:
    bool remove_index(int index);

    public:
        int chunk_index;
        int iban; //inventory_block_array_num
        int ibam; //inventory_block_array_max
        struct InventoryBlock* iba; //inventory_block_array

    ChunkItemContainer() :
        chunk_index(0), iban(0), ibam(2)
    {
        iba = (struct InventoryBlock*) malloc(ibam*sizeof(struct InventoryBlock));
        for (int i=0; i<ibam; i++)
        {
            iba[i].container_id = NULL_CONTAINER;
            iba[i].container_type = NULL_CONTAINER_TYPE;
        }
    }

    ~ChunkItemContainer()
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
            iba[i].container_type = NULL_CONTAINER_TYPE;
        }
    }
    #endif

    bool _remove(int index);

    bool remove(int x, int y, int z);
    bool remove(ItemContainerID container_id);

    void get_container_location(ItemContainerID container_id, int position[3])
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

    bool add(int x, int y, int z, ItemContainerType container_type, ItemContainerID container_id);

    ItemContainerID get(int x, int y, int z)
    {
        for (int i=0; i<iban; i++)
            if (x == iba[i].x && y == iba[i].y && z == iba[i].z)
                return iba[i].container_id;
        return NULL_CONTAINER;
    }

    #if DC_SERVER
    void send_chunk_item_containers(ClientID client_id);
    void send_reset_chunk_item_containers(ClientID client_id);
    #endif
};




}   // t_map
