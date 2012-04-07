#include "inventory.hpp"

#include <c_lib/agent/agent.hpp> 
#include <c_lib/state/server_state.hpp>
#if DC_CLIENT
#include <c_lib/SDL/SDL_functions.h>
#endif

/* Inventory Properties */

#if DC_CLIENT
void InventoryProperties::get_sprite_data(struct Draw::SpriteData* data)
{
    data->index = this->sprite_index;
    const float spacing = 64.0f;
    data->x = inventory->screen.x + spacing * slot;
    data->y = inventory->screen.y;
    data->z = inventory->screen.z;
}
#endif

/* Inventory Contents */

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
        msg.slot = i;
        msg.sendToClient(client_id);
    }
}

/* Network Interface */

Inventory::Inventory(int id)
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

void Inventory::attach_to_owner()
{
    Agent_state* a = STATE::agent_list->get(this->get_owner());
    if (a == NULL)
    {
        printf("WARNING: Inventroy::attach_to_owner() -- agent %d not found\n", this->get_owner());
        return;
    }
    if (a->status.inventory != NULL)
        printf("WARNING: reassigned agent inventory\n");
    a->status.inventory = this;
}

