#include "silver.hpp"

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
static void set_silver_gemstone_properties(Object* object)
{   // attach components
    #if DC_CLIENT
    const int n_components = 4;
    #endif
    #if DC_SERVER
    const int n_components = 3;
    #endif
    object->init(n_components);

    #if DC_CLIENT
    add_component_to_object(object, COMPONENT_BILLBOARD_SPRITE);
    #endif
    add_component_to_object(object, COMPONENT_VERLET);
    add_component_to_object(object, COMPONENT_PICKUP);
    add_component_to_object(object, COMPONENT_TTL);

    object->tick = &tick_silver_gemstone;
    //object->update = NULL;

    object->create = create_packet_momentum;
    object->state = state_packet_momentum;
}

Object* create_silver_gemstone()
{
    // initialize object
    ObjectType type = OBJECT_GEMSTONE_SILVER;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_silver_gemstone_properties(obj);
    return obj;
}

void ready_silver_gemstone(Object* object)
{
    #if DC_SERVER
    // broadcast create
    #endif
}

void die_silver_gemstone(Object* object)
{
    #if DC_SERVER
    using Components::PickupComponent;
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->broadcast();
    #endif
}

void tick_silver_gemstone(Object* object)
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

//void update_silver_gemstone(Object* object)
//{
    // update for draw
//}

} // Objects
