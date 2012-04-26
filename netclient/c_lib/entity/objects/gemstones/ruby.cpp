#include "ruby.hpp"

#include <entity/objects/gemstones/constants.hpp>
#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/verlet.hpp>
#if DC_CLIENT
#include <entity/components/draw/billboard_sprite.hpp>
#endif

namespace Objects
{

// private
static void set_ruby_gemstone_properties(Object* object)
{   // attach components
    
    #if DC_CLIENT
    using Components::BillboardSpriteComponent;
    BillboardSpriteComponent* sprite = (BillboardSpriteComponent*)add_component_to_object(object, COMPONENT_BILLBOARD_SPRITE);
    sprite->sprite_index = RUBY_GEMSTONE_SPRITE_INDEX;
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

    object->tick = &tick_pickup_sprite;
    //object->update = NULL;

    object->create = create_packet_momentum;
    object->state = state_packet_momentum;
}

void create_ruby_gemstone(Object* object)
{
    set_ruby_gemstone_properties(object);
}

} // Objects
