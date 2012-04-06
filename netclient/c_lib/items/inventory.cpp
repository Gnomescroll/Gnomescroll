#include "inventory.hpp"


/* Network Interface */

void InventoryNetworkInterface::sendToClientCreate(int client_id)
{
    inventory_create_StoC msg;
    bool success = inventory_create_message(&msg, (Inventory*)this);
    if (!success) return;
    msg.sendToClient(client_id);
}

void InventoryNetworkInterface::broadcastCreate()
{
    inventory_create_StoC msg;
    bool success = inventory_create_message(&msg, (Inventory*)this);
    if (!success) return;
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
    inventory_destroy_message(&msg, this->state()->id);
    msg.broadcast();
}


/* Inventory Object */

void Inventory::add_contents(int* ids, Object_types* types, int n_contents)
{
    if (n_contents <= 0)
    {
        printf("ERROR: Inventory::add_contents() -- n_contents %d invalid\n", n_contents);
        return;
    }
    ObjectPolicyInterface* obj;
    for (int i=0; i<n_contents; i++)
    {
        if (ids[i] == EMPTY_SLOT)
        {
            if (this->contents[i] != NULL)
                this->ct--;
            this->contents[i] = NULL;
            continue;
        }

        obj = STATE::object_list->get(types[i], ids[i]);
        if (this->contents[i] == NULL && obj != NULL)
            this->ct++;
        this->contents[i] = obj;
    }
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
    // fill contents
    obj->add_contents((int*)content_ids, (Object_types*)content_types, x*y);

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
