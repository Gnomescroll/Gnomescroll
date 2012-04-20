#include "turquoise.hpp"

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

// private
static void set_turquoise_gemstone_properties(Object* object)
{   // attach components
    #if DC_CLIENT
    const int n_components = 4;
    #endif
    #if DC_SERVER
    const int n_components = 3;
    #endif
    object->init(n_components);

    #if DC_CLIENT
    using Components::BillboardSpriteComponent;
    BillboardSpriteComponent* sprite = (BillboardSpriteComponent*)add_component_to_object(object, COMPONENT_BILLBOARD_SPRITE);
    sprite->sprite_index = TURQUOISE_GEMSTONE_SPRITE_INDEX;
    sprite->scale = GEMSTONE_SCALE;
    #endif

    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;
    
    VerletPhysicsComponent* physics = (VerletPhysicsComponent*)add_component_to_object(object, COMPONENT_VERLET);
    physics->mass = GEMSTONE_MASS;
    physics->damp = GEMSTONE_DAMP;
    
    PickupComponent* pickup = (PickupComponent*)add_component_to_object(object, COMPONENT_PICKUP);
    pickup->pickup_radius = GEMSTONE_PICKUP_RADIUS;
    
    TTLHealthComponent* health = (TTLHealthComponent*)add_component_to_object(object, COMPONENT_TTL);
    health->ttl_max = GEMSTONE_TTL;


    object->tick = &tick_turquoise_gemstone;
    //object->update = NULL;

    object->create = create_packet_momentum;
    object->state = state_packet_momentum;
}

Object* create_turquoise_gemstone()
{
    // initialize object
    ObjectType type = OBJECT_GEMSTONE_TURQUOISE;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_turquoise_gemstone_properties(obj);
    return obj;
}

void ready_turquoise_gemstone(Object* object)
{
    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_turquoise_gemstone(Object* object)
{
    #if DC_SERVER
    using Components::PickupComponent;
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->broadcast();
    #endif
}

void tick_turquoise_gemstone(Object* object)
{
    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;
    
    // update for physics
    VerletPhysicsComponent* verlet = (VerletPhysicsComponent*)object->get_component(COMPONENT_VERLET);
    verlet->bounce();

    #if DC_SERVER
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->tick(object);
    #endif

    TTLHealthComponent* ttl = (TTLHealthComponent*)object->get_component(COMPONENT_TTL);
    ttl->tick();
}

//void update_turquoise_gemstone(Object* object)
//{
    // update for draw
//}

} // Objects
