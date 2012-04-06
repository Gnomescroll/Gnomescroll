#include "inventory.hpp"


/* Network Interface */

void InventoryNetworkInterface::sendToClientCreate(int client_id)
{
    inventory_create_StoC msg;
    ObjectState* state = object->state();
    printf("going to send inventory\n");
    printf("id,type = %d,%d\n", state->id, state->type);
    bool success = inventory_create_message(&msg,
        state->id, state->type,
        object->x, object->y, object->owner,
        object->contents
    );
    if (!success) return;
    msg.sendToClient(client_id);
}

void InventoryNetworkInterface::broadcastCreate()
{
    inventory_create_StoC msg;
    ObjectState* state = object->state();
    bool success = inventory_create_message(&msg,
        state->id, state->type,
        object->x, object->y, object->owner,
        object->contents
    );
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
    inventory_destroy_message(&msg, object->state()->id);
    msg.broadcast();
}


/* Inventory Object */

void Inventory::add_contents_from_packet(uint16_t* ids, uint8_t* types, int n_contents)
{
    if (n_contents <= 0)
    {
        printf("ERROR: Inventory::add_contents() -- n_contents %d invalid\n", n_contents);
        return;
    }
    ObjectPolicyInterface* obj;
    for (int i=0; i<n_contents; i++)
    {
        if ((int)ids[i] == EMPTY_SLOT)
        {
            if (this->contents[i] != NULL)
                this->ct--;
            this->contents[i] = NULL;
            continue;
        }

        obj = STATE::object_list->get((Object_types)types[i], (int)ids[i]);
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
    obj->add_contents_from_packet(content_ids, content_types, x*y);

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
