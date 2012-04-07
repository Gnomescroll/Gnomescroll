#include "inventory.hpp"

#include <c_lib/agent/agent.hpp> 
#include <c_lib/state/server_state.hpp>

/* Inventory Contents */

void InventoryContents::sendToClient(int inventory_id, int client_id)
{
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i].id == EMPTY_SLOT)
            continue;
        add_item_to_inventory_StoC msg;
        msg.inventory_id = inventory_id;
        msg.id = this->objects[i].id;
        msg.type = this->objects[i].type;
        msg.slot = i;
        msg.sendToClient(client_id);
    }
}

/* Network Interface */

void Inventory::sendToClientCreate(int client_id)
{
    inventory_create_StoC msg;
    ObjectState* state = this->state();
    inventory_create_message(&msg,
        state->id, state->type,
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
        state->id, state->type,
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

void Inventory::sendToClientAdd(int id, Object_types type, int slot)
{
    #if DC_SERVER
    printf("send to client add\n");
    Agent_state* agent = ServerState::agent_list->get(this->get_owner());
    if (agent == NULL) return;
    add_item_to_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.id = id;
    msg.type = type;
    msg.slot = slot;
    msg.sendToClient(agent->client_id);
    #endif
}

void Inventory::broadcastAdd(int id, Object_types type, int slot)
{
    #if DC_SERVER
    add_item_to_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.id = id;
    msg.type = type;
    msg.slot = slot;
    msg.broadcast();
    #endif
}

void Inventory::sendToClientRemove(int slot)
{
    #if DC_SERVER
    Agent_state* agent = ServerState::agent_list->get(this->get_owner());
    if (agent == NULL) return;
    remove_item_from_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.slot = slot;
    msg.sendToClient(agent->client_id);
    #endif
}

void Inventory::broadcastRemove(int slot)
{
    #if DC_SERVER
    remove_item_from_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.slot = slot;
    msg.broadcast();
    #endif
}
