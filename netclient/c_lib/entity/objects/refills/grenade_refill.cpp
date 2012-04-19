#include "grenade_refill.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/verlet.hpp>

namespace Objects
{

// private
static void set_grenade_refill_properties(Object* obj)
{   // attach components
    const int n_components = 4;
    obj->init(n_components);
    add_component_to_object(obj, COMPONENT_BILLBOARD_SPRITE);
    add_component_to_object(obj, COMPONENT_VERLET);
    add_component_to_object(obj, COMPONENT_PICKUP);
    add_component_to_object(obj, COMPONENT_TTL);

    obj->tick = &tick_grenade_refill;
    //obj->update = NULL;

    obj->create = create_packet_momentum;
    obj->state = state_packet_momentum;
}

Object* create_grenade_refill()
{
    // initialize object
    ObjectType type = OBJECT_GRENADE_REFILL;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_grenade_refill_properties(obj);
    return obj;
}

void ready_grenade_refill(Object* object)
{
    #if DC_SERVER
    // broadcast create
    #endif
}

void die_grenade_refill(Object* object)
{
    #if DC_SERVER
    using Components::PickupComponent;
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->broadcast();
    #endif
}

void tick_grenade_refill(Object* object)
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

//void update_grenade_refill(Object* object)
//{
    // update for draw
//}

} // Objects
