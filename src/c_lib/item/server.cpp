#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace Item
{

// subscriptions

void subscribe_agent_to_item(int agent_id, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    
    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    item->subscribers.add(a->client_id);
}

void unsubscribe_agent_from_item(int agent_id, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    
    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    item->subscribers.remove(a->client_id);
}

void unsubscribe_all_from_item(ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    
    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    item->subscribers.remove_all();
}

// packets

static bool pack_item_create(Item* item, item_create_StoC* msg)
{
    GS_ASSERT(item->id != NULL_ITEM);
    GS_ASSERT(item->type != NULL_ITEM_TYPE);
    GS_ASSERT(item->group != NULL_ITEM_GROUP);
    GS_ASSERT(item->stack_size > 0);

    if (item->id == NULL_ITEM) return false;
    if (item->type == NULL_ITEM_TYPE) return false;
    if (item->group == NULL_ITEM_GROUP) return false;
    if (item->stack_size <= 0) return false;
    
    msg->id = item->id;
    msg->type = item->type;
    msg->group = item->group;
    msg->durability = item->durability;
    msg->stack_size = item->stack_size;

    return true;
}

void broadcast_item_create(ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return;
    
    item_create_StoC msg;
    if (!pack_item_create(item, &msg)) return;
    msg.broadcast();
}

void send_item_create(int client_id, ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return;

    item_create_StoC msg;
    if (!pack_item_create(item, &msg)) return;
    msg.sendToClient(client_id);
}

void send_item_create(ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return;
    if (item->subscribers.n <= 0) return;

    item_create_StoC msg;
    if (!pack_item_create(item, &msg)) return;
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.n);
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
    Item* item = get_item(item_id);
    if (item == NULL) return;

    item_state_StoC msg;
    pack_item_state(item, &msg);
    msg.sendToClient(client_id);
}

void send_item_state(ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return;
    if (item->subscribers.n <= 0) return;
    
    item_state_StoC msg;
    pack_item_state(item, &msg);
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.n);
}

void broadcast_item_state(ItemID item_id)
{
    Item* item = get_item(item_id);
    if (item == NULL) return;

    item_state_StoC msg;
    pack_item_state(item, &msg);
    msg.broadcast();
}

void send_item_destroy(int client_id, ItemID item_id)
{
    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    
    item_destroy_StoC msg;
    msg.id = item_id;
    msg.sendToClient(client_id);
}

void broadcast_item_destroy(ItemID item_id)
{
    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);

    item_destroy_StoC msg;
    msg.id = item_id;
    msg.broadcast();
}

}   // Item
