#pragma once

#if DC_CLIENT
# error Don't include this file in the client
#endif

#include <item/container/net/StoC.hpp>

namespace ItemContainer
{

// does not manage subscriptions -- only use in special cases (like item destroy handler)
void remove_item_from_hand(AgentID agent_id);

// hand/container -> container/hand
void transfer_item_between_containers(ItemID item_id, ItemContainerID container_id_a, int slot_a, ItemContainerID container_id_b, int slot_b);
void transfer_item_from_container_to_hand(ItemID item_id, ItemContainerID container_id, int slot, AgentID agent_id);
void transfer_item_from_hand_to_container(ItemID item_id, ItemContainerID container_id, int slot, AgentID agent_id);
bool swap_item_between_hand_and_container(AgentID agent_id, ItemContainerID container_id, int slot);

// brand new item -> container/hand
bool transfer_free_item_to_container(ItemID item_id, ItemContainerID container_id, int slot);
bool transfer_free_item_to_hand(ItemID item_id, AgentID agent_id);

// particle -> container/hand
bool transfer_particle_to_container(ItemID item_id, ItemParticleID particle_id, ItemContainerID container_id, int slot);
void transfer_particle_to_hand(ItemID item_id, ItemParticleID particle_id, AgentID agent_id);

// hand -> particle (throw)
void transfer_hand_to_particle(AgentID agent_id);

// everything -> particle is handled by create_item_particle

// packets

void send_container_assign(ClientID client_id, ItemContainerID container_id);
void send_container_create(ClientID client_id, ItemContainerID container_id);
void send_container_delete(ClientID client_id, ItemContainerID container_id);
void broadcast_container_create(ItemContainerID container_id);
void broadcast_container_delete(ItemContainerID container_id);

void send_container_lock(ClientID client_id, ItemContainerID container_id);
void broadcast_container_lock(ItemContainerID container_id);
void broadcast_container_unlock(ItemContainerID container_id, int unlocking_agent_id);

void send_container_state(ClientID client_id, ItemContainerID container_id);

void send_container_item_create(ClientID client_id, ItemID item_id, ItemContainerID container_id, int slot);

void send_container_insert(ClientID client_id, ItemID item_id, ItemContainerID container_id, int slot)
{
    IF_ASSERT(item_id == NULL_ITEM) return;
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    IF_ASSERT(slot == NULL_SLOT) return;
    insert_item_in_container_StoC msg;
    msg.container_id = container_id;
    msg.item_id = item_id;
    msg.slot = slot;
    msg.sendToClient(client_id);
}

void send_container_remove(ClientID client_id, ItemContainerID container_id, int slot)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    IF_ASSERT(slot == NULL_SLOT) return;
    remove_item_from_container_StoC msg;
    msg.container_id = container_id;
    msg.slot = slot;
    msg.sendToClient(client_id);
}

void send_hand_insert(ClientID client_id, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);

    insert_item_in_hand_StoC msg;
    msg.item_id = item_id;
    msg.sendToClient(client_id);
}

void send_hand_remove(ClientID client_id)
{
    remove_item_from_hand_StoC msg;
    msg.sendToClient(client_id);
}

void send_container_failed_action(ClientID client_id, int event_id)
{
    container_action_failed_StoC msg;
    msg.event_id = event_id;
    msg.sendToClient(client_id);
}

void send_container_close(AgentID agent_id, ItemContainerID container_id);
void send_container_open(AgentID agent_id, ItemContainerID container_id);
void send_open_container_failed(ClientID client_id, ItemContainerID container_id, int event_id);

void send_smelter_fuel(ItemContainerID container_id);
void send_smelter_progress(ItemContainerID container_id);

// transactions on free containers (not private containers)
bool agent_open_container(AgentID agent_id, ItemContainerID container_id);
void agent_close_container(AgentID agent_id, ItemContainerID container_id);
void agent_close_container_silent(AgentID agent_id, ItemContainerID container_id);  // doesnt send_container_close

void unsubscribe_agent_from_container_contents(AgentID agent_id, ItemContainerID container_id);


}   // ItemContainer
