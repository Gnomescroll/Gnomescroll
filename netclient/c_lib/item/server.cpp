#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace Item
{

static void pack_item_create(Item* item, item_create_StoC* msg)
{
    assert(item->id != NULL_ITEM);
    assert(item->type != NULL_ITEM_TYPE);
    assert(item->group != NULL_ITEM_GROUP);
    assert(item->stack_size > 0);
    msg->id = item->id;
    msg->type = item->type;
    msg->group = item->group;
    msg->durability = item->durability;
    msg->stack_size = item->stack_size;
}

void broadcast_item_create(ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return;
    item_create_StoC msg;
    pack_item_create(item, &msg);
    msg.broadcast();
}

Item* send_item_create(int client_id, ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return NULL;
    item_create_StoC msg;
    pack_item_create(item, &msg);
    msg.sendToClient(client_id);
    return item;
}

static void pack_item_state(Item* item, item_state_StoC* msg)
{
    msg->id = item->id;
    msg->stack_size = item->stack_size;
    msg->durability = item->durability;
    msg->energy = item->energy;
}

void send_item_state(int client_id, ItemID item_id)
{
    Item* item = item_list->get((ItemID)item_id);
    if (item == NULL) return;
    item_state_StoC msg;
    pack_item_state(item, &msg);
    msg.sendToClient(client_id);
}

void broadcast_item_state(ItemID item_id)
{
    Item* item = item_list->get((ItemID)item_id);
    if (item == NULL) return;
    item_state_StoC msg;
    pack_item_state(item, &msg);
    msg.broadcast();
}

void send_item_destroy(int client_id, ItemID item_id)
{
    item_destroy_StoC msg;
    msg.id = item_id;
    msg.sendToClient(client_id);
}

void broadcast_item_destroy(ItemID item_id)
{
    item_destroy_StoC msg;
    msg.id = item_id;
    msg.broadcast();
}

}   // Item
