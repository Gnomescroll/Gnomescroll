#pragma once

#if DC_SERVER

namespace Item
{

void broadcast_item_create(ItemID item_id);
void broadcast_item_destroy(int client_id, ItemID item_id);
class Item* send_item_create(int client_id, ItemID item_id);
void send_container_item_create(int client_id, ItemID item_id, int container_id, int slot);

// packets
void send_container_failed_action(int client_id, int event_id)
{
    container_action_failed_StoC msg;
    msg.event_id = event_id;
    msg.sendToClient(client_id);
}

// transaction
int auto_add_item_to_container(ItemContainer* container, ItemID item_id)
{
    assert(container != NULL);

    // if can stack with item in container
    // stack with it
    // else
    // add to empty slot

    int slot = container->get_empty_slot();
    if (slot == NULL_SLOT) return NULL_SLOT;

    container->insert_item(slot, item_id);
    return slot;
}

}

#endif
