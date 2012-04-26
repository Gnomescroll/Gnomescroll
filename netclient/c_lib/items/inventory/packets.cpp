#include "packets.hpp"

#include <c_lib/items/inventory/interface.hpp>

#include <c_lib/items/inventory/handlers.hpp>

/* Packets */

#if DC_CLIENT
inline void inventory_create_StoC::handle()
{
    Inventory* obj = Items::get_inventory(this->id);
    if (obj == NULL)
    {   // create [ NOTE: do not use get_or_create; will need to call born() method if created ]
        obj = Items::create_inventory((ObjectType)this->type, this->id);
        if (obj == NULL)
        {
            printf("WARNING: inventory_create_StoC::handle() -- failed to create inventory %d\n", id);
            return;
        }
        Items::init_inventory_dimensions(obj);
    }
    else if (obj->type != this->type)
        printf("WARNING: inventory_create_StoC::handle() inventory %d type mismatch: %d (local) %d (remote)\n", id, obj->type, this->type);

    obj->owner = owner;
    #if DC_CLIENT
    Items::received_inventory_handler(obj);
    #endif
}

inline void inventory_destroy_StoC::handle()
{
    Items::destroy_inventory(this->id);
}

inline void add_item_to_inventory_StoC::handle()
{
    Inventory* obj = Items::get_inventory(this->inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: add_item_to_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->add(id, (ObjectType)type, stack, slot);
}

inline void remove_item_from_inventory_StoC::handle()
{
    Inventory* obj = Items::get_inventory(this->inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: add_item_to_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->remove(slot);
}

inline void swap_item_in_inventory_StoC::handle()
{
    Inventory* obj = Items::get_inventory(this->inventory_id);
    if (obj == NULL)
    {
        printf("WARNING: swap_item_in_inventory_StoC::handle() -- inventory %d not found\n", inventory_id);
        return;
    }
    obj->swap(slota, slotb);
}

inline void swap_item_between_inventory_StoC::handle()
{
    Inventory* inva = Items::get_inventory(this->inventorya);
    if (inva == NULL) return;
    Inventory* invb = Items::get_inventory(this->inventoryb);
    if (invb == NULL) return;

    InventorySlot* item = inva->get_item(slota);
    invb->add(item->item_id, item->item_type, item->count, slotb);
    inva->remove(slota);
}

inline void add_item_to_inventory_CtoS::handle() {}
inline void remove_item_from_inventory_CtoS::handle() {}
inline void swap_item_in_inventory_CtoS::handle() {}
inline void swap_item_between_inventory_CtoS::handle() {}
#endif

#if DC_SERVER
inline void inventory_create_StoC::handle() {}
inline void inventory_destroy_StoC::handle() {}
inline void add_item_to_inventory_StoC::handle() {}
inline void remove_item_from_inventory_StoC::handle() {}
inline void swap_item_in_inventory_StoC::handle() {}
inline void swap_item_between_inventory_StoC::handle() {}

inline void add_item_to_inventory_CtoS::handle()
{
    Agent_state* agent = NetServer::agents[client_id];
    if (agent == NULL)
    {
        printf("add_item_to_inventory_CtoS::handle() -- agent not found for client %d\n", client_id);
        return;
    }
    Inventory* inv = Items::get_inventory(this->inventory_id);
    if (inv == NULL) return;
    if (inv->owner != agent->id) return;
    //ObjectPolicyInterface* obj = ServerState::object_list->get((ObjectType)type, id);
    //if (obj == NULL) return;    // TODO -- make sure this object will exist in this use case!!
    const int stack_size = 1;
    inv->add_action(id, (ObjectType)type, stack_size, slot);
}

inline void remove_item_from_inventory_CtoS::handle()
{
    Agent_state* agent = NetServer::agents[client_id];
    if (agent == NULL)
    {
        printf("remove_item_from_inventory_CtoS::handle() -- agent not found for client %d\n", client_id);
        return;
    }
    
    Inventory* inv = Items::get_inventory(this->inventory_id);
    if (inv == NULL) return;
    if (inv->owner != agent->id) return;
    if (!inv->can_remove((int)slot)) return;
    
    InventorySlot* item = inv->get_item(slot);
    if (item == NULL) return;
    if (!item->empty())
    {   // create new item
        Objects::Object* obj = Objects::create(item->item_type);
        if (obj != NULL)
        {
            Vec3 position = agent->get_center();
            const float velocity = 1.0f;
            Vec3 forward = vec3_scalar_mult(agent->s.forward_vector(), velocity);

            using Components::PhysicsComponent;
            PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            if (physics != NULL)
            {
                physics->set_position(position);
                physics->set_momentum(forward);
            }

            using Components::PickupComponent;
            PickupComponent* pickup = (PickupComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PICKUP);
            if (pickup != NULL) pickup->was_dropped();

            Objects::ready(obj);
        }
    }

    if (!inv->remove_action(slot)) printf("ERROR: inventory remove_action failed to occur -- but can_remove() had passed\n");
}

inline void swap_item_in_inventory_CtoS::handle()
{
    Agent_state* agent = NetServer::agents[client_id];
    if (agent == NULL)
    {
        printf("swap_item_in_inventory_CtoS::handle() -- agent not found for client %d\n", client_id);
        return;
    }
    Inventory* inv = Items::get_inventory(this->inventory_id);
    if (inv == NULL) return;
    if (inv->owner != agent->id) return;
    inv->swap_action(slota, slotb);
}

inline void swap_item_between_inventory_CtoS::handle()
{
    Agent_state* agent = NetServer::agents[client_id];
    if (agent == NULL)
    {
        printf("swap_item_between_inventory_CtoS::handle() -- agent not found for client %d\n", client_id);
        return;
    }
    
    Inventory* inva = Items::get_inventory(this->inventorya);
    if (inva == NULL) return;
    if (inva->owner != agent->id) return;
    Inventory* invb = Items::get_inventory(this->inventoryb);
    if (invb == NULL) return;
    if (invb->owner != agent->id) return;

    if (!inva->can_remove(slota)) return;
    InventorySlot* item = inva->get_item(slota);
    if (!invb->can_add(item->item_type, slotb)) return;

    if (!invb->add(item->item_id, item->item_type, item->count, slotb)) printf("ERROR ADDING NEW ITEM!!\n");
    if (!inva->remove(slota)) printf("ERROR REMOVING ITEM!!\n");

    swap_item_between_inventory_StoC msg;
    msg.inventorya = inventorya;
    msg.slota = slota;
    msg.inventoryb = inventoryb;
    msg.slotb = slotb;
    msg.sendToClient(client_id);
}

#endif
