#include "chunk_special.hpp"

#include <item/container/_interface.hpp>
#include <item/container/container.hpp>
#if DC_SERVER
#include <item/container/server.hpp>
#endif

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
    #endif

    ItemContainer::container_block_destroyed(iba[i].container_id, iba[i].x, iba[i].y, iba[i].z);
    
    _remove(i);
}

void CHUNK_ITEM_CONTAINER::remove(int container_id)
{
    // find container
    int i;
    for(i=0; i<iban; i++)
        if(container_id == iba[i].container_id)
            break;
    GS_ASSERT(i < iban);   // did not find
    if (i >= iban) return;

    this->remove_index(i);
}

void CHUNK_ITEM_CONTAINER::remove(int x, int y, int z)
{
    x = translate_point(x);
    y = translate_point(y);

    // find container
    int i;
    for(i=0; i<iban; i++)
        if(x == iba[i].x && y == iba[i].y && z == iba[i].z)
            break;
    GS_ASSERT(i < iban);   // did not find
    if (i >= iban) return;

    this->remove_index(i);
}

void CHUNK_ITEM_CONTAINER::add(int x, int y, int z, int container_type, int container_id)
{
    x = translate_point(x);
    y = translate_point(y);
    GS_ASSERT(x >= 0 && x < XMAX && y >= 0 && y < YMAX && z >= 0 && z < ZMAX);
    GS_ASSERT(container_type != CONTAINER_TYPE_NONE);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(ibam < MAP_CHUNK_XDIM*MAP_CHUNK_YDIM);
    if (ibam >= MAP_CHUNK_XDIM*MAP_CHUNK_YDIM) return;
    
    if(iban == ibam)
    {
        int o_ibam = ibam;
        ibam *= 2;
        ibam = (ibam >= MAP_CHUNK_XDIM*MAP_CHUNK_YDIM) ? (MAP_CHUNK_XDIM*MAP_CHUNK_YDIM) - 1 : ibam;
        GS_ASSERT(ibam >= o_ibam);
        if (ibam < o_ibam) return;
        GS_ASSERT(iban < ibam);
        if (iban >= ibam) return;
        if (o_ibam != ibam)
        {
            struct inventory_block* new_iba = (struct inventory_block*) realloc(iba, ibam*sizeof(struct inventory_block));
            GS_ASSERT(new_iba != NULL);
            if (new_iba == NULL)
            {   // error handling
                free(iba);
                iba = NULL;
                ibam = 0;
            }
            else iba = new_iba;
        }
        GS_ASSERT(iba != NULL);
        if (iba == NULL) return;
    }

    iba[iban].x = x;
    iba[iban].y = y;
    iba[iban].z = z;
    iba[iban].container_type = container_type;
    iba[iban].container_id = container_id;
    iban++;

    #if DC_SERVER
    map_history->container_block_create(chunk_index, x, y, z, container_type, container_id);

    GS_ASSERT(container_id != NULL_CONTAINER);
    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    container->chunk = this->chunk_index;
    #endif

    // need to create the container here
    
}


#if DC_SERVER
void CHUNK_ITEM_CONTAINER::send_chunk_item_containers(int client_id)
{
    class container_block_chunk_reset_StoC reset_msg;
    reset_msg.chunk_index = chunk_index;
    reset_msg.sendToClient(client_id);

    for(int i=0; i < iban; i++)
    {
        class container_block_create_StoC msg;
        msg.x = iba[i].x;
        msg.y = iba[i].y;
        msg.z = iba[i].z;
        msg.container_type = iba[i].container_type;
        msg.container_id = iba[i].container_id;
        msg.sendToClient(client_id);

        ItemContainer::send_container_create(client_id, iba[i].container_id);
        ItemContainer::send_container_state(client_id, iba[i].container_id);
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
