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
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    
    Agent* a = Agents::get_any_agent(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    GS_ASSERT(item->subscribers.n == 0);
    bool add = item->subscribers.add(a->client_id);
    GS_ASSERT(add);
    if (!add) return;
    GS_ASSERT(item->subscribers.n == 1);

    send_item_create(a->client_id, item->id);
}

void unsubscribe_agent_from_item(AgentID agent_id, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    
    Agent* a = Agents::get_any_agent(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    GS_ASSERT(item->subscribers.n == 1);
    bool rm = item->subscribers.remove(a->client_id);
    GS_ASSERT(rm);
    if (!rm) return;
    GS_ASSERT(item->subscribers.n == 0);

    //printf("Unsubscribed %d from %d\n", a->client_id, item->id);

    // send destroy packet to client
    send_item_destroy(a->client_id, item->id);
}

void unsubscribe_all_from_item(ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    
    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    if (item->subscribers.n == 0) return;

    //for (unsigned int i=0; i<item->subscribers.n; i++)
        //printf("Unsubscribed %d from %d\n", item->subscribers.subscribers[i], item->id);

    GS_ASSERT(item->subscribers.n == 1);
    send_item_destroy(item->id);    // sends to all subscribers
    item->subscribers.remove_all();
}

// packets

static bool pack_item_create(Item* item, item_create_StoC* msg)
{
    GS_ASSERT(item->id != NULL_ITEM);
    GS_ASSERT(item->type != NULL_ITEM_TYPE);
    GS_ASSERT(item->stack_size > 0);

    if (item->id == NULL_ITEM) return false;
    if (item->type == NULL_ITEM_TYPE) return false;
    if (item->stack_size <= 0) return false;
    
    msg->id = item->id;
    msg->type = item->type;
    msg->durability = item->durability;
    msg->stack_size = item->stack_size;

    return true;
}

static void send_item_create(ClientID client_id, ItemID item_id)
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
    GS_ASSERT_LIMIT(item->subscribers.n == 1, 60);

    item_create_StoC msg;
    if (!pack_item_create(item, &msg)) return;
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.n);
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
    if (item == NULL) return;
    if (item->subscribers.n <= 0) return;
    GS_ASSERT_LIMIT(item->subscribers.n == 1, 60);

    item_state_StoC msg;
    pack_item_state(item, &msg);
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.n);
}

static void send_item_destroy(ClientID client_id, ItemID item_id)
{
    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    
    item_destroy_StoC msg;
    msg.id = item_id;
    msg.sendToClient(client_id);
}

void send_item_destroy(ItemID item_id)
{
    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item->subscribers.n <= 0) return;
    GS_ASSERT_LIMIT(item->subscribers.n == 1, 60);
    
    item_destroy_StoC msg;
    msg.id = item_id;
    msg.sendToClients(item->subscribers.subscribers, item->subscribers.n);
}

}   // Item
