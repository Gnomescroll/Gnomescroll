#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/container/net/StoC.hpp>

namespace ItemContainer
{

// packets

void send_container_assign(int client_id, int container_id);
void send_container_create(int client_id, int container_id);
void send_container_delete(int client_id, int container_id);
void broadcast_container_create(int container_id);
void broadcast_container_delete(int container_id);

void send_container_item_create(int client_id, ItemID item_id, int container_id, int slot);

void send_container_insert(int client_id, ItemID item_id, int container_id, int slot)
{
    assert(item_id != NULL_ITEM);
    assert(container_id != NULL_CONTAINER);
    assert(slot != NULL_SLOT);
    insert_item_in_container_StoC msg;
    msg.container_id = container_id;
    msg.item_id = item_id;
    msg.slot = slot;
    msg.sendToClient(client_id);
}

void send_container_remove(int client_id, int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);
    assert(slot != NULL_SLOT);
    remove_item_from_container_StoC msg;
    msg.container_id = container_id;
    msg.slot = slot;
    msg.sendToClient(client_id);
}

void send_hand_insert(int client_id, ItemID item_id)
{
    assert(item_id != NULL_ITEM);

    insert_item_in_hand_StoC msg;
    msg.item_id = item_id;
    msg.sendToClient(client_id);
}

void send_hand_remove(int client_id)
{
    remove_item_from_hand_StoC msg;
    msg.sendToClient(client_id);
}

void send_container_failed_action(int client_id, int event_id)
{
    container_action_failed_StoC msg;
    msg.event_id = event_id;
    msg.sendToClient(client_id);
}

void send_container_close(int agent_id, int container_id);
void send_container_open(int agent_id, int container_id);
void send_open_container_failed(int client_id, int container_id, int event_id);

// transactions
bool agent_open_container(int agent_id, int container_id);
void agent_close_container(int agent_id, int container_id);

}   // ItemContainer
