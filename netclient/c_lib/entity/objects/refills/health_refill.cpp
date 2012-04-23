#include "health_refill.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/verlet.hpp>
#include <c_lib/entity/components/draw/billboard_sprite.hpp>
#include <c_lib/entity/components/pickup.hpp>
#include <c_lib/entity/components/health/ttl.hpp>

namespace Objects
{

void load_health_refill_data()
{
    load_refill_data(OBJECT_HEALTH_REFILL);
}


// private
static void set_health_refill_properties(Object* object)
{   // attach components
    #if DC_CLIENT
    const int n_components = 4;
    #endif
    #if DC_SERVER
    const int n_components = 3;
    #endif
    object->init(n_components);

    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;

    #if DC_CLIENT
    using Components::BillboardSpriteComponent;
    BillboardSpriteComponent* sprite = (BillboardSpriteComponent*)add_component_to_object(object, COMPONENT_BILLBOARD_SPRITE);
    sprite->scale = REFILL_ITEM_SCALE;
    sprite->sprite_index = HEALTH_REFILL_SPRITE_INDEX;
    #endif

    VerletPhysicsComponent* physics = (VerletPhysicsComponent*)add_component_to_object(object, COMPONENT_VERLET);
    physics->mass = REFILL_ITEM_MASS;
    physics->damp = REFILL_ITEM_DAMP;
    
    PickupComponent* pickup = (PickupComponent*)add_component_to_object(object, COMPONENT_PICKUP);
    pickup->pickup_radius = REFILL_ITEM_PICKUP_RADIUS;
    
    TTLHealthComponent* health = (TTLHealthComponent*)add_component_to_object(object, COMPONENT_TTL);
    health->ttl_max = REFILL_ITEM_TTL;

    object->tick = &tick_health_refill;
    //object->update = NULL;

    object->create = create_packet_momentum;
    object->state = state_packet_momentum;
}

Object* create_health_refill()
{
    ObjectType type = OBJECT_HEALTH_REFILL;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_health_refill_properties(obj);
    return obj;
}

void ready_health_refill(Object* object)
{
    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_health_refill(Object* object)
{
    #if DC_SERVER
    using Components::PickupComponent;
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->broadcast();
    #endif
}

void tick_health_refill(Object* object)
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
    #endif

    TTLHealthComponent* ttl = (TTLHealthComponent*)object->get_component(COMPONENT_TTL);
    ttl->tick();
}

//void update_health_refill(Object* object)
//{
    // update for draw
//}

} // Objects
