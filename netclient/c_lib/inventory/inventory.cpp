#include "inventory.hpp"

#include <agent/agent.hpp>

namespace Items
{

void init_inventory_dimensions(class BaseInventory* inventory)
{
    switch (inventory->type)
    {
        case OBJECT_AGENT_INVENTORY:
            inventory->init(AGENT_INVENTORY_X, AGENT_INVENTORY_Y);
            break;
        case OBJECT_AGENT_TOOLBELT:
            inventory->init(AGENT_TOOLBELT_X, AGENT_TOOLBELT_Y);
            break;
        case OBJECT_CRAFTING_BENCH:
            inventory->init(CRAFTING_BENCH_X, CRAFTING_BENCH_Y);
            break;
        case OBJECT_NANITE_INVENTORY:
            inventory->init(NANITE_INVENTORY_X, NANITE_INVENTORY_Y);
            break;
            
        default:
            printf("WARNING: Items::create_inventory() -- unknown inventory type %d\n", inventory->type);
            assert(false);
            break;
    }
}

void move_inventory_item_to_world(Agent_state* agent, InventorySlot* item)
{
    assert(item != NULL);
    if (item->empty()) return;
    Objects::Object* obj = Objects::create(item->item_type);
    if (obj == NULL) return;

    Vec3 position = agent->get_center();
    float velocity = 1.0f + ((randf() - 0.5f) / 20.0f);
    Vec3 forward = vec3_scalar_mult(agent->s.forward_vector(), velocity);
    float bias = randf() / 100.0f;
    forward = vec3_bias(forward, bias);

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

    using Components::StackableComponent;
    StackableComponent* stack = (StackableComponent*)obj->get_component_interface(COMPONENT_INTERFACE_STACKABLE);
    if (stack != NULL) stack->count = item->count;

    Objects::ready(obj);
}


} // Items
