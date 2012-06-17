#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/container/net/StoC.hpp>

namespace ItemContainer
{

// does not manage subscriptions -- only use in special cases (like item destroy handler)
void remove_item_from_hand(int agent_id);

// hand/container -> container/hand
void transfer_item_between_containers(ItemID item_id, int container_id_a, int slot_a, int container_id_b, int slot_b);
void transfer_item_from_container_to_hand(ItemID item_id, int container_id, int slot, int agent_id);
void transfer_item_from_hand_to_container(ItemID item_id, int container_id, int slot, int agent_id);

// brand new item -> container/hand
void transfer_free_item_to_container(ItemID item_id, int container_id, int slot);
void transfer_free_item_to_hand(ItemID item_id, int agent_id);

// particle -> container/hand
void transfer_particle_to_container(ItemID item_id, int particle_id, int container_id, int slot);
void transfer_particle_to_hand(ItemID item_id, int particle_id, int agent_id);

// hand -> particle (throw)
void transfer_hand_to_particle(int agent_id);

// everything -> particle is handled by create_item_particle

// packets

void send_container_assign(int client_id, int container_id);
void send_container_create(int client_id, int container_id);
void send_container_delete(int client_id, int container_id);
void broadcast_container_create(int container_id);
void broadcast_container_delete(int container_id);

void send_container_lock(int client_id, int container_id);
void broadcast_container_lock(int container_id);
void broadcast_container_unlock(int container_id, int unlocking_agent_id);

void send_container_state(int client_id, int container_id);

void send_container_item_create(int client_id, ItemID item_id, int container_id, int slot);

void send_container_insert(int client_id, ItemID item_id, int container_id, int slot)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);
    if (item_id == NULL_ITEM) return;
    if (container_id == NULL_CONTAINER) return;
    if (slot == NULL_SLOT) return;
    insert_item_in_container_StoC msg;
    msg.container_id = container_id;
    msg.item_id = item_id;
    msg.slot = slot;
    msg.sendToClient(client_id);
}

void send_container_remove(int client_id, int container_id, int slot)
{    
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);
    if (container_id == NULL_CONTAINER) return;
    if (slot == NULL_SLOT) return;
    remove_item_from_container_StoC msg;
    msg.container_id = container_id;
    msg.slot = slot;
    msg.sendToClient(client_id);
}

void send_hand_insert(int client_id, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);

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

void send_smelter_fuel(int container_id);
void send_smelter_progress(int container_id);

// transactions on free containers (not private containers)
bool agent_open_container(int agent_id, int container_id);
void agent_close_container(int agent_id, int container_id);

void unsubscribe_agent_from_container_contents(int agent_id, int container_id);


}   // ItemContainer
