#include "health_refill.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/verlet.hpp>

namespace Objects
{

Object* create_health_refill()
{
    // initialize object
    ObjectType type = OBJECT_HEALTH_REFILL;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;

    // attach components
    //const int n_components = 3;
    const int n_components = 4;
    obj->init(n_components);
    add_component_to_object(obj, COMPONENT_BILLBOARD_SPRITE);
    add_component_to_object(obj, COMPONENT_VERLET);
    add_component_to_object(obj, COMPONENT_PICKUP);
    add_component_to_object(obj, COMPONENT_TTL);

    obj->tick = &tick_health_refill;
    //obj->update = NULL;

    return obj;
}

void ready_health_refill(Object* object)
{

}

void die_health_refill(Object* object)
{

}

void tick_health_refill(Object* object)
{
    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;
    
    // update for physics
    VerletPhysicsComponent* verlet = (VerletPhysicsComponent*)object->get_component(COMPONENT_VERLET);
    verlet->bounce();

    //tickPickup(state, this->get_position(), &this->pickup);
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->tick(object);

    //tickTTL(state);
    TTLHealthComponent* ttl = (TTLHealthComponent*)object->get_component(COMPONENT_TTL);
    ttl->tick();
}

//void update_health_refill(Object* object)
//{
    // update for draw
//}

} // Objects
