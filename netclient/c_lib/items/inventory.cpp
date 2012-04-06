#include "inventory.hpp"

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
    this->contents.sendToClient(client_id);
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


/* Packets */

#if DC_CLIENT
inline void inventory_create_StoC::handle()
{
    bool create = false;
    Inventory* obj = (Inventory*)ClientState::object_list->get(OBJ_TYPE_INVENTORY, id);
    if (obj == NULL)
    {   // create [ NOTE: do not use get_or_create; will need to call born() method if created ]
        create = true;
        obj = (Inventory*)ClientState::object_list->create(OBJ_TYPE_INVENTORY, id);
        if (obj == NULL)
        {
            printf("WARNING: inventory_create_StoC::handle() -- failed to create inventory %d\n", id);
            return;
        }
    }

    // set dimensions
    obj->set_dimensions(x,y);
    // set owner
    obj->set_owner(owner);  // TODO : owned properties

    if (create)
        obj->born();
}

inline void inventory_destroy_StoC::handle()
{
    ClientState::object_list->destroy(OBJ_TYPE_INVENTORY, id);
}

#endif

#if DC_SERVER
inline void inventory_create_StoC::handle() {}
inline void inventory_destroy_StoC::handle() {}
#endif
