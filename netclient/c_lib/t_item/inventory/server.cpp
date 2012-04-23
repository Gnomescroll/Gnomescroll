#include "server.hpp"

#if DC_SERVER

/* Inventory */

void Inventory::sendToClientCreate(int client_id)
{
    inventory_create_StoC msg;
    inventory_create_message(&msg,
        this->id, this->type, 
        this->contents.x, this->contents.y, owner
    );
    msg.sendToClient(client_id);

    // send create packets for everything in the inventory
    this->sendContentsToClient(client_id);
}

void Inventory::broadcastCreate()
{
    inventory_create_StoC msg;
    inventory_create_message(&msg,
        this->id, this->type,
        this->contents.x, this->contents.y, owner
    );
    msg.broadcast();
}

void Inventory::sendToClientState(int client_id)
{
}

void Inventory::broadcastState()
{
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

#endif
