#include "packets.hpp"

#include <c_lib/items/pickup.hpp>

/* Packets */

#if DC_CLIENT
inline void inventory_create_StoC::handle()
{
    bool create = false;
    Inventory* obj = (Inventory*)ClientState::object_list->get(OBJECT_INVENTORY, id);
    if (obj == NULL)
    {   // create [ NOTE: do not use get_or_create; will need to call born() method if created ]
        create = true;
        obj = (Inventory*)ClientState::object_list->create(OBJECT_INVENTORY, id);
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
    #if DC_CLIENT   // TODO
    attach_inventory_to_owner(obj, obj->get_owner());
    #endif
    if (create)
        obj->born();
}

inline void inventory_destroy_StoC::handle()
{
    ClientState::object_list->destroy(OBJECT_INVENTORY, id);
}

inline void add_item_to_inventory_StoC::handle()
{
    Inventory* obj = (Inventory*)ClientState::object_list->get(OBJECT_INVENTORY, inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: add_item_to_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->add(id, (ObjectType)type, stack, slot); /// TODO -- GET STACK MAX FROM ITEM_TYPE
    //printf("added item %d,%d to inventory slot %d\n", id, type, slot);
}

inline void remove_item_from_inventory_StoC::handle()
{
    Inventory* obj = (Inventory*)ClientState::object_list->get(OBJECT_INVENTORY, inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: add_item_to_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->remove(slot);
    //printf("removed item from inventory slot %d\n", slot);
}

inline void swap_item_in_inventory_StoC::handle()
{
    Inventory* obj = (Inventory*)ClientState::object_list->get(OBJECT_INVENTORY, inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: swap_item_in_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->swap(slota, slotb);
    //printf("swapped slots %d,%d in inventory %d\n", slota, slotb, inventory_id);
}

inline void add_item_to_inventory_CtoS::handle() {}
inline void remove_item_from_inventory_CtoS::handle() {}
inline void swap_item_in_inventory_CtoS::handle() {}

#endif

#if DC_SERVER
inline void inventory_create_StoC::handle() {}
inline void inventory_destroy_StoC::handle() {}
inline void add_item_to_inventory_StoC::handle() {}
inline void remove_item_from_inventory_StoC::handle() {}
inline void swap_item_in_inventory_StoC::handle() {}

inline void add_item_to_inventory_CtoS::handle()
{
    Agent_state* agent = NetServer::agents[client_id];
    if (agent == NULL)
    {
        printf("add_item_to_inventory_CtoS::handle() -- agent not found for client %d\n", client_id);
        return;
    }
    Inventory* inv = (Inventory*)ServerState::object_list->get(OBJECT_INVENTORY, inventory_id);
    if (inv == NULL) return;
    if (inv->get_owner() != agent->id) return;
    ObjectPolicyInterface* obj = ServerState::object_list->get((ObjectType)type, id);
    if (obj == NULL) return;    // TODO -- make sure this object will exist in this use case!!
    inv->add_action(id, (ObjectType)type, obj->get_stack_size(), slot);
}

inline void remove_item_from_inventory_CtoS::handle()
{
    Agent_state* agent = NetServer::agents[client_id];
    if (agent == NULL)
    {
        printf("remove_item_from_inventory_CtoS::handle() -- agent not found for client %d\n", client_id);
        return;
    }
    Inventory* inv = (Inventory*)ServerState::object_list->get(OBJECT_INVENTORY, inventory_id);
    if (inv == NULL) return;
    if (inv->get_owner() != agent->id) return;
    if (!inv->can_remove((int)slot)) return;
    InventoryProperties* item = inv->get(slot);
    if (item == NULL) return;
    if (!item->empty())
    {   // create new item
        ItemDrops::PickupObject* obj = (ItemDrops::PickupObject*)ServerState::object_list->create(item->item_type);
        if (obj == NULL) return;
        Vec3 position = agent->get_center();
        const float velocity = 1.0f;
        Vec3 forward = vec3_scalar_mult(agent->s.forward_vector(), velocity);
        obj->set_position(position.x, position.y, position.z);
        obj->set_momentum(forward.x, forward.y, forward.z);
        obj->born();
        obj->pickup.was_dropped();
        obj->state()->ttl_max = 30 * 60; // 1 minute
    }

    if (!inv->remove_action(slot)) printf("ERROR: inventory remove_action failed to occur -- but can_remove() had passed\n");
}

inline void swap_item_in_inventory_CtoS::handle()
{
    printf("Received swap slot: invid %d slota %d slotb %d\n", inventory_id, slota, slotb);
    Agent_state* agent = NetServer::agents[client_id];
    if (agent == NULL)
    {
        printf("swap_item_in_inventory_CtoS::handle() -- agent not found for client %d\n", client_id);
        return;
    }
    Inventory* inv = (Inventory*)ServerState::object_list->get(OBJECT_INVENTORY, inventory_id);
    if (inv == NULL) return;
    if (inv->get_owner() != agent->id) return;
    inv->swap_action(slota, slotb);
}

#endif
