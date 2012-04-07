#include "base.hpp"


#include <c_lib/agent/agent.hpp> 
#include <c_lib/state/server_state.hpp>
#if DC_CLIENT
#include <c_lib/SDL/SDL_functions.h>
#endif


BaseInventory::BaseInventory(int id)
{
    this->_state.id = id;
    this->owned_properties.owner = NO_AGENT;
    // todo
    #if DC_CLIENT
    this->screen.x = _xresf/3;
    this->screen.y = _yresf - 64;
    this->screen.z = -0.1f;
    #endif
}

/* Network Interface */

void BaseInventory::sendToClientCreate(int client_id)
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

void BaseInventory::broadcastCreate()
{
    inventory_create_StoC msg;
    ObjectState* state = this->state();
    inventory_create_message(&msg,
        state->id, state->type,
        this->contents.x, this->contents.y, this->get_owner()
    );
    msg.broadcast();
}

void BaseInventory::sendToClientState(int client_id)
{
}

void BaseInventory::broadcastState()
{
}

void BaseInventory::broadcastDeath()
{
    inventory_destroy_StoC msg;
    inventory_destroy_message(&msg, this->state()->id);
    msg.broadcast();
}

void BaseInventory::sendToClientAdd(int id, Object_types type, int slot)
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

void BaseInventory::broadcastAdd(int id, Object_types type, int slot)
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

void BaseInventory::sendToClientRemove(int slot)
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

void BaseInventory::broadcastRemove(int slot)
{
    #if DC_SERVER
    remove_item_from_inventory_StoC msg;
    msg.inventory_id = this->_state.id;
    msg.slot = slot;
    msg.broadcast();
    #endif
}
