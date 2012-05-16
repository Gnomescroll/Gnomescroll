#include "chunk_special.hpp"

#include <item/container/_interface.hpp>
#include <item/container/container.hpp>

namespace t_map
{

void CHUNK_ITEM_CONTAINER::remove_index(int i)
{
    #if DC_SERVER
    map_history->container_block_delete(chunk_index, iba[i].container_id);

    // dump contents into world
    //int subscriber_count;
    //unsigned short* subscribers = map_history->get_subscribers(&subscriber_count)
    // dont send to subscribers only here -- there may be different subscriber infrastructure for this later
    ItemContainer::container_block_destroyed(iba[i].container_id, iba[i].x, iba[i].y, iba[i].z);
    #endif
    
    _remove(i);
}

void CHUNK_ITEM_CONTAINER::remove(int container_id)
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

    this->remove_index(i);
}

void CHUNK_ITEM_CONTAINER::remove(int x, int y, int z)
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

    this->remove_index(i);
}

void CHUNK_ITEM_CONTAINER::add(int x, int y, int z, int container_type, int container_id)
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

    #if DC_SERVER
    map_history->container_block_create(chunk_index, x, y, z, container_type, container_id);

    assert(container_id != NULL_CONTAINER);
    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    assert(container != NULL);
    container->chunk = this->chunk_index;
    #endif    
}


#if DC_SERVER
void CHUNK_ITEM_CONTAINER::send_chunk_item_containers(int client_id)
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
        msg.container_id = iba[i].container_id;
        msg.sendToClient(client_id);

        ItemContainer::send_container_create(client_id, iba[i].container_id);
    }
}

void CHUNK_ITEM_CONTAINER::send_reset_chunk_item_containers(int client_id)
{
    class container_block_chunk_reset_StoC msg;
    msg.chunk_index = chunk_index;
    msg.sendToClient(client_id);

    for (int i=0; i< iban; i++)
        ItemContainer::send_container_delete(client_id, iba[i].container_id);
}

#endif

}   // t_map
