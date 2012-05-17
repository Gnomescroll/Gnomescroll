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
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    msg->container_id = container->id;
    msg->container_type = container->type;
    msg->chunk = container->chunk;
    return true;
}

void send_container_create(int client_id, int container_id)
{
    assert(container_id != NULL_CONTAINER);
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.sendToClient(client_id);
}

void broadcast_container_create(int container_id)
{
    assert(container_id != NULL_CONTAINER);
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.broadcast();
}

void send_container_delete(int client_id, int container_id)
{
    assert(container_id != NULL_CONTAINER);
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(client_id);
}

void broadcast_container_delete(int container_id)
{
    assert(container_id != NULL_CONTAINER);
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.broadcast();
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
    assert(opened_containers != NULL);
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(container_id != NULL_CONTAINER);

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return false;

    if (!container->can_be_opened_by(a->id)) return false;

    // release currently opened container
    if (opened_containers[agent_id] != NULL_CONTAINER)
    {
        ItemContainerInterface* opened = get_container(opened_containers[agent_id]);
        assert(opened != NULL);
        opened->unlock(a->id);
        opened_containers[agent_id] = NULL_CONTAINER;
    }

    // place player lock on container if we want
    container->lock(a->id);
    opened_containers[agent_id] = container->id;

    // send container contents to player
    send_container_contents(a->id, a->client_id, container_id);
    return true;
}

void agent_close_container(int agent_id, int container_id)
{
    assert(opened_containers != NULL);
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(container_id != NULL_CONTAINER);
    
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    container->unlock(agent_id);
    opened_containers[agent_id] = NULL_CONTAINER;
}

}   // ItemContainer
