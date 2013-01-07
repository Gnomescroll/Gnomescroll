#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <agent/_interface.hpp>

namespace Item
{

static void send_item_create(ClientID client_id, ItemID item_id);
static void send_item_destroy(ClientID client_id, ItemID item_id);

// subscriptions

void subscribe_agent_to_item(AgentID agent_id, ItemID item_id)
{
    IF_ASSERT(item_id == NULL_ITEM) return;
    GS_ASSERT(isValid(agent_id));
    
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;

    GS_ASSERT(item->subscribers.count == 0);
    bool add = item->subscribers.add(a->client_id);
    IF_ASSERT(!add) return;
    GS_ASSERT(item->subscribers.count == 1);

    send_item_create(a->client_id, item->id);
}

void unsubscribe_agent_from_item(AgentID agent_id, ItemID item_id)
{
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;

    // TODO -- allow more subscribers under certain contexts (public containers)
    if (item->subscribers.count <= 0) return;
    GS_ASSERT(item->subscribers.count == 1);
    bool rm = item->subscribers.remove(a->client_id);
    IF_ASSERT(!rm) return;
    GS_ASSERT(item->subscribers.count == 0);

    //printf("Unsubscribed %d from %d\n", a->client_id, item->id);

    // send destroy packet to client
    send_item_destroy(a->client_id, item->id);
}

void unsubscribe_all_from_item(ItemID item_id)
{
    IF_ASSERT(item_id == NULL_ITEM) return;
    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;

    if (item->subscribers.count <= 0) return;
    GS_ASSERT(item->subscribers.count == 1);
    send_item_destroy(item->id);    // sends to all subscribers
    item->subscribers.remove_all();
}

// packets

static bool pack_item_create(Item* item, item_create_StoC* msg)
{
    IF_ASSERT(item->id == NULL_ITEM) return false;
    IF_ASSERT(item->type == NULL_ITEM_TYPE) return false;
    IF_ASSERT(item->stack_size <= 0) return false;
    
    msg->id = item->id;
    msg->type = item->type;
    msg->durability = item->durability;
    msg->stack_size = item->stack_size;

    return true;
}

static void send_item_create(ClientID client_id, ItemID item_id)
{
    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;

    item_create_StoC msg;
    if (!pack_item_create(item, &msg)) return;
    msg.sendToClient(client_id);

    int max_charges = get_max_charges(item->type);
    if (max_charges > 0)
        send_item_charges(item_id);
}

void send_item_create(ItemID item_id)
{
    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;
    GS_ASSERT(item->subscribers.count == 1);

    item_create_StoC msg;
    if (!pack_item_create(item, &msg)) return;
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.count);
}

static void pack_item_state(Item* item, item_state_StoC* msg)
{
    msg->id = item->id;
    msg->stack_size = item->stack_size;
    msg->durability = item->durability;
}

void send_item_state(ItemID item_id)
{
    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;
    if (item->subscribers.count <= 0) return;
    GS_ASSERT(item->subscribers.count == 1);

    item_state_StoC msg;
    pack_item_state(item, &msg);
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.count);

    int max_charges = get_max_charges(item->type);
    if (max_charges > 0)
        send_item_charges(item_id);
}

static void send_item_destroy(ClientID client_id, ItemID item_id)
{
    GS_ASSERT(get_item(item_id) != NULL);
    
    item_destroy_StoC msg;
    msg.id = item_id;
    msg.sendToClient(client_id);
}

void send_item_destroy(ItemID item_id)
{
    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;
    if (item->subscribers.count <= 0) return;
    GS_ASSERT(item->subscribers.count == 1);
    
    item_destroy_StoC msg;
    msg.id = item_id;
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.count);
}

void send_item_charges(ItemID item_id)
{
    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return;
    if (item->subscribers.count <= 0) return;
    GS_ASSERT(item->subscribers.count == 1);

    item_charges_StoC msg;
    msg.id = item_id;
    msg.charges = item->charges;
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.count);
}

}   // Item
