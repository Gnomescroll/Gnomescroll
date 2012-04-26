#include "jade.hpp"

#include <c_lib/entity/objects/gemstones/constants.hpp>
#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/verlet.hpp>
#if DC_CLIENT
#include <c_lib/entity/components/draw/billboard_sprite.hpp>
#endif

namespace Objects
{

void load_jade_gemstone_data()
{
    load_gemstone_data(OBJECT_GEMSTONE_JADE);
}

// private
static void set_jade_gemstone_properties(Object* object)
{   // attach components

    #if DC_CLIENT
    using Components::BillboardSpriteComponent;
    BillboardSpriteComponent* sprite = (BillboardSpriteComponent*)add_component_to_object(object, COMPONENT_BILLBOARD_SPRITE);
    sprite->sprite_index = JADE_GEMSTONE_SPRITE_INDEX;
    sprite->scale = GEMSTONE_SCALE;
    #endif

    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;
    using Components::StackableComponent;
    
    VerletPhysicsComponent* physics = (VerletPhysicsComponent*)add_component_to_object(object, COMPONENT_VERLET);
    physics->mass = GEMSTONE_MASS;
    physics->damp = GEMSTONE_DAMP;
    
    PickupComponent* pickup = (PickupComponent*)add_component_to_object(object, COMPONENT_PICKUP);
    pickup->pickup_radius = GEMSTONE_PICKUP_RADIUS;
    
    TTLHealthComponent* health = (TTLHealthComponent*)add_component_to_object(object, COMPONENT_TTL);
    health->ttl_max = GEMSTONE_TTL;

    StackableComponent* stack = (StackableComponent*)add_component_to_object(object, COMPONENT_STACKABLE);
    stack->max = GEMSTONE_STACK_MAX;

    object->tick = &tick_jade_gemstone;
    //object->update = NULL;

    object->create = create_packet_momentum;
    object->state = state_packet_momentum;
}

Object* create_jade_gemstone()
{
    // initialize object
    ObjectType type = OBJECT_GEMSTONE_JADE;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_jade_gemstone_properties(obj);
    return obj;
}

void ready_jade_gemstone(Object* object)
{
    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_jade_gemstone(Object* object)
{
    #if DC_SERVER
    using Components::PickupComponent;
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    if (pickup->picked_up) pickup->broadcast();
    else object->broadcastDeath();
    #endif
}

void tick_jade_gemstone(Object* object)
{
    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;
    
    // update for physics
    VerletPhysicsComponent* verlet = (VerletPhysicsComponent*)object->get_component(COMPONENT_VERLET);
    verlet->bounce();

    #if DC_SERVER
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->tick();

    TTLHealthComponent* ttl = (TTLHealthComponent*)object->get_component(COMPONENT_TTL);
    ttl->tick();
    #endif
}

//void update_jade_gemstone(Object* object)
//{
    // update for draw
//}

} // Objects
