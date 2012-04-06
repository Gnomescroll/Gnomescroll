#include "inventory.hpp"

/* Network Interface */

void InventoryNetworkInterface::sendToClientCreate(int client_id)
{
    inventory_create_StoC msg;
    ObjectState* state = object->state();
    inventory_create_message(&msg,
        state->id, state->type,
        object->contents.x, object->contents.y, object->get_owner(),
        (InventoryProperties**)object->contents.objects
    );
    msg.sendToClient(client_id);
}

void InventoryNetworkInterface::broadcastCreate()
{
    inventory_create_StoC msg;
    ObjectState* state = object->state();
    inventory_create_message(&msg,
        state->id, state->type,
        object->contents.x, object->contents.y, object->get_owner(),
        (InventoryProperties**)object->contents.objects
    );
    msg.broadcast();
}

void InventoryNetworkInterface::sendToClientState(int client_id)
{
}

void InventoryNetworkInterface::broadcastState()
{
}

void InventoryNetworkInterface::broadcastDeath()
{
    inventory_destroy_StoC msg;
    inventory_destroy_message(&msg, object->state()->id);
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
