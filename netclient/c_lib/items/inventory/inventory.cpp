#include "inventory.hpp"

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
        obj->init(x,y);
    }
    else if (obj->dimension_mismatch(x,y))
        printf("WARNING: inventory_create_StoC::handle() inventory %d known but dimension mismatch\n", id);

    obj->set_owner(owner);
    if (create)
        obj->born();
}

inline void inventory_destroy_StoC::handle()
{
    ClientState::object_list->destroy(OBJ_TYPE_INVENTORY, id);
}

inline void add_item_to_inventory_StoC::handle()
{
    Inventory* obj = (Inventory*)ClientState::object_list->get(OBJ_TYPE_INVENTORY, inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: add_item_to_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->add(id, (Object_types)type, slot);
}

inline void remove_item_from_inventory_StoC::handle()
{
    Inventory* obj = (Inventory*)ClientState::object_list->get(OBJ_TYPE_INVENTORY, inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: add_item_to_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->remove(slot);
}

#endif

#if DC_SERVER
inline void inventory_create_StoC::handle() {}
inline void inventory_destroy_StoC::handle() {}

inline void add_item_to_inventory_StoC::handle() {}
inline void remove_item_from_inventory_StoC::handle() {}

#endif
