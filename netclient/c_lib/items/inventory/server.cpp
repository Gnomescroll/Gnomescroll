#include "server.hpp"

#if DC_SERVER

#include <c_lib/agent/agent.hpp>

/* Inventory */

// dumps contents
void Inventory::remove_all_action()
{
    Agent_state* agent = ServerState::agent_list->get(this->owner);
    if (agent == NULL) return;  // TODO -- get inventory state other ways
    
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i].item_id == EMPTY_SLOT) continue;
        InventorySlot* item = this->get_item(i);
        assert(item != NULL);
        if (!this->can_remove(i)) continue;
        Items::move_inventory_item_to_world(agent, item);
        this->remove_action(i);
    }
}

void Inventory::sendContentsToClient(int client_id)
{
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i].item_id == EMPTY_SLOT)
            continue;
        add_item_to_inventory_StoC msg;
        msg.inventory_id = this->id;
        msg.id = this->objects[i].item_id;
        msg.type = this->objects[i].item_type;
        msg.stack = this->objects[i].count;
        msg.slot = i;
        msg.sendToClient(client_id);
    }
}


void Inventory::sendToClientCreate(int client_id)
{
    inventory_create_StoC msg;
    inventory_create_message(&msg, this->id, this->type, owner);
    msg.sendToClient(client_id);

    // send create packets for everything in the inventory
    this->sendContentsToClient(client_id);
}

void Inventory::broadcastCreate()
{
    inventory_create_StoC msg;
    inventory_create_message(&msg, this->id, this->type, owner);
    msg.broadcast();
}

void Inventory::broadcastDeath()
{
    inventory_destroy_StoC msg;
    inventory_destroy_message(&msg, this->id);
    msg.broadcast();
}

void Inventory::sendToClientAdd(int id, ObjectType type, int stack_size, int slot)
{
    Agent_state* agent = ServerState::agent_list->get(owner);
    if (agent == NULL) return;
    add_item_to_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.id = id;
    msg.type = type;
    msg.stack = stack_size;
    msg.slot = slot;
    msg.sendToClient(agent->client_id);
}

void Inventory::broadcastAdd(int id, ObjectType type, int stack_size, int slot)
{
    add_item_to_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.id = id;
    msg.type = type;
    msg.stack = stack_size;
    msg.slot = slot;
    msg.broadcast();
}

void Inventory::sendToClientRemove(int slot)
{
    Agent_state* agent = ServerState::agent_list->get(owner);
    if (agent == NULL) return;
    remove_item_from_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.slot = slot;
    msg.sendToClient(agent->client_id);
}

void Inventory::broadcastRemove(int slot)
{
    remove_item_from_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.slot = slot;
    msg.broadcast();
}

void Inventory::sendToClientSwap(int slota, int slotb)
{
    Agent_state* agent = ServerState::agent_list->get(owner);
    if (agent == NULL) return;
    swap_item_in_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.slota = slota;
    msg.slotb = slotb;
    msg.sendToClient(agent->client_id);
}

void Inventory::broadcastSwap(int slota, int slotb)
{
    swap_item_in_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.slota = slota;
    msg.slotb = slotb;
    msg.broadcast();
}

void Inventory::sendToClientMergeStack(int slota, int slotb, int count)
{
    Agent_state* agent = ServerState::agent_list->get(this->owner);
    if (agent == NULL) return;
    merge_stack_in_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.slota = slota;
    msg.slotb = slotb;
    msg.count = count;
    msg.sendToClient(agent->client_id);
}

void Inventory::broadcastMergeStack(int slota, int slotb, int count)
{
    merge_stack_in_inventory_StoC msg;
    msg.inventory_id = this->id;
    msg.slota = slota;
    msg.slotb = slotb;
    msg.count = count;
    msg.broadcast();
}


#endif
