#include "server.hpp"

#if DC_SERVER

namespace Item
{

void broadcast_item_create(ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return;
    item_create_StoC msg;
    msg.id = item->id;
    msg.type = item->type;
    msg.group = item->group;
    msg.durability = item->durability;
    msg.stack_size = item->stack_size;
    msg.broadcast();
}

Item* send_item_create(int client_id, ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return NULL;
    item_create_StoC msg;
    msg.id = item->id;
    msg.type = item->type;
    msg.group = item->group;
    msg.durability = item->durability;
    msg.stack_size = item->stack_size;
    msg.sendToClient(client_id);
    return item;
}

void send_container_item_create(int client_id, ItemID item_id, int container_id, int slot)
{
    Item* item = send_item_create(client_id, item_id);
    if (item == NULL) return;

    insert_item_in_container_StoC msg;
    msg.container_id = container_id;
    msg.item_id = item->id;
    msg.slot = slot;
    msg.sendToClient(client_id);
}

void broadcast_item_destroy(int client_id, ItemID item_id)
{
    item_destroy_StoC msg;
    msg.item_id = item_id;
    msg.sendToClient(client_id);
}


}

#endif
