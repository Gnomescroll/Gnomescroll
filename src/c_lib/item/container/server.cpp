#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace ItemContainer
{

/* Network */

//  tell client to assign container to an agent
void send_container_assign(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    class assign_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

static bool pack_container_create(int container_id, create_item_container_StoC* msg)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    msg->container_id = container->id;
    msg->container_type = container->type;
    msg->chunk = container->chunk;
    return true;
}

void send_container_create(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.sendToClient(client_id);
}

void broadcast_container_create(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.broadcast();
}

void send_container_delete(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(client_id);
}

void broadcast_container_delete(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.broadcast();
}

static bool pack_container_lock(int container_id, lock_container_StoC* msg)
{
    GS_ASSERT(msg != NULL);
    if (msg == NULL) return false;

    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    ASSERT_VALID_AGENT_ID(container->owner);
    if (container->owner == NO_AGENT) return false;
    
    msg->container_id = container->id;
    msg->agent_id = container->owner;
    return true;
}

void send_container_lock(int client_id, int container_id)
{
    lock_container_StoC msg;
    if (!pack_container_lock(container_id, &msg)) return;
    msg.sendToClient(client_id);
}

void broadcast_container_lock(int container_id)
{
    lock_container_StoC msg;
    if (!pack_container_lock(container_id, &msg)) return;
    msg.broadcast();
}

void broadcast_container_unlock(int container_id, int unlocking_agent_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;
    GS_ASSERT(container->owner == NO_AGENT);
    if (container->owner != NO_AGENT) return;

    unlock_container_StoC msg;
    msg.container_id = container->id;
    msg.agent_id = unlocking_agent_id;
    msg.broadcast();
}

void send_container_state(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    if (container->owner != NO_AGENT) send_container_lock(client_id, container_id);
}

void send_container_item_create(int client_id, ItemID item_id, int container_id, int slot)
{
    Item::Item* item = Item::send_item_create(client_id, item_id);
    if (item == NULL) return;
    send_container_insert(client_id, item_id, container_id, slot);
}

void send_container_close(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    
    close_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(a->client_id);
}

void send_container_open(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    
    open_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(a->client_id);
}

void send_open_container_failed(int client_id, int container_id, int event_id)
{
    open_container_failed_StoC msg;
    msg.container_id = container_id;
    msg.event_id = event_id;
    msg.sendToClient(client_id);
}

// transactions
bool agent_open_container(int agent_id, int container_id)
{
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers == NULL) return false;
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;

    if (!agent_can_access_container(agent_id, container_id)) return false;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return false;

    // release currently opened container
    if (opened_containers[agent_id] != NULL_CONTAINER)
    {
        ItemContainerInterface* opened = get_container(opened_containers[agent_id]);
        GS_ASSERT(opened != NULL);
        if (opened == NULL) return false;
        opened_containers[agent_id] = NULL_CONTAINER;
        bool did_unlock = opened->unlock(a->id);
        GS_ASSERT(did_unlock);
        broadcast_container_unlock(container_id, agent_id);
    }

    // place player lock on container if we want
    bool did_lock = container->lock(a->id);
    GS_ASSERT(did_lock);
    GS_ASSERT(opened_containers[agent_id] == NULL_CONTAINER);
    opened_containers[agent_id] = container->id;
    // send new lock to players
    broadcast_container_lock(container->id);
    
    // send container contents to player
    send_container_contents(a->id, a->client_id, container_id);
    return true;
}

void agent_close_container(int agent_id, int container_id)
{
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers == NULL) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    
    // throw anything in hand
    if (agent_hand_list[agent_id] != NULL_ITEM)
    {
        if (a != NULL) send_hand_remove(a->client_id);
        ItemParticle::throw_agent_item(agent_id, agent_hand_list[agent_id]);
    }
    agent_hand_list[agent_id] = NULL_ITEM;
    
    opened_containers[agent_id] = NULL_CONTAINER;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    bool did_unlock = container->unlock(agent_id);
    GS_ASSERT(did_unlock);
    broadcast_container_unlock(container->id, agent_id);
}

}   // ItemContainer
