#include "packets.hpp"

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
    printf("added item %d,%d to inventory slot %d\n", id, type, slot);
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
