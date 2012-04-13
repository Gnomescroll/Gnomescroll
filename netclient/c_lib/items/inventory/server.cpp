#include "server.hpp"

#if DC_SERVER

/* Network interfaces */

/* Contents */

void InventoryContents::sendToClient(int inventory_id, int client_id)
{
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i].item_id == EMPTY_SLOT)
            continue;
        add_item_to_inventory_StoC msg;
        msg.inventory_id = inventory_id;
        msg.id = this->objects[i].item_id;
        msg.type = this->objects[i].item_type;
        msg.subtype = this->objects[i].item_subtype;
        msg.slot = i;
        msg.sendToClient(client_id);
    }
}

/* Inventory */

void Inventory::sendToClientCreate(int client_id)
{
    inventory_create_StoC msg;
    ObjectState* state = this->state();
    inventory_create_message(&msg,
        state->id, state->type, state->subtype, 
        this->contents.x, this->contents.y, this->get_owner()
    );
    msg.sendToClient(client_id);

    // send create packets for everything in the inventory
    this->sendContentsToClient(client_id);
}

void Inventory::broadcastCreate()
{
    inventory_create_StoC msg;
    ObjectState* state = this->state();
    inventory_create_message(&msg,
        state->id, state->type, state->subtype,
        this->contents.x, this->contents.y, this->get_owner()
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
    inventory_destroy_message(&msg, this->state()->id);
    msg.broadcast();
}

void Inventory::sendToClientAdd(int id, Object_types type, int subtype, int slot)
{
    Agent_state* agent = ServerState::agent_list->get(this->get_owner());
    if (agent == NULL) return;
    add_item_to_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.id = id;
    msg.type = type;
    msg.subtype = subtype;
    msg.slot = slot;
    msg.sendToClient(agent->client_id);
}

void Inventory::broadcastAdd(int id, Object_types type, int subtype, int slot)
{
    add_item_to_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.id = id;
    msg.type = type;
    msg.subtype = subtype;
    msg.slot = slot;
    msg.broadcast();
}

void Inventory::sendToClientRemove(int slot)
{
    Agent_state* agent = ServerState::agent_list->get(this->get_owner());
    if (agent == NULL) return;
    remove_item_from_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.slot = slot;
    msg.sendToClient(agent->client_id);
}

void Inventory::broadcastRemove(int slot)
{
    remove_item_from_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.slot = slot;
    msg.broadcast();
}

void Inventory::sendToClientSwap(int slota, int slotb)
{
    Agent_state* agent = ServerState::agent_list->get(this->get_owner());
    if (agent == NULL) return;
    swap_item_in_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.slota = slota;
    msg.slotb = slotb;
    msg.sendToClient(agent->client_id);
}

void Inventory::broadcastSwap(int slota, int slotb)
{
    swap_item_in_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.slota = slota;
    msg.slotb = slotb;
    msg.broadcast();

}

#endif
