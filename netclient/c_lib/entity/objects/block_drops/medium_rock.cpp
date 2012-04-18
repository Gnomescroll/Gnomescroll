#include "medium_rock.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/verlet.hpp>

namespace Objects
{
    
static void set_medium_rock_block_drop_properties(Object* obj)
{
    const int n_components = 4;
    obj->init(n_components);
    add_component_to_object(obj, COMPONENT_TEXTURED_VOXEL);
    add_component_to_object(obj, COMPONENT_VERLET);
    add_component_to_object(obj, COMPONENT_PICKUP);
    add_component_to_object(obj, COMPONENT_TTL);

    obj->tick = &tick_medium_rock_block_drop;
}

Object* create_medium_rock_block_drop()
{
    ObjectType type = OBJECT_MEDIUM_ROCK_BLOCK_DROP;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_medium_rock_block_drop_properties(obj);
    return obj;
}

void ready_medium_rock_block_drop(Object* object)
{
    // init voxel for rendering
    #if DC_CLIENT
    using Components::TexturedVoxelComponent;
    TexturedVoxelComponent* voxel = (TexturedVoxelComponent*)object->get_component(COMPONENT_TEXTURED_VOXEL);
    voxel->init();   // sets vectors
    #endif

    #if DC_SERVER
    // broadcast create
    #endif
}

void die_medium_rock_block_drop(Object* object)
{
    #if DC_SERVER
    using Components::PickupComponent;
    PickupComponent* pickup = (PickupComponent*)object->get_component(COMPONENT_PICKUP);
    pickup->broadcast();
    #endif
}

void tick_medium_rock_block_drop(Object* object)
{
    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;

    // rotation animation
    #if DC_CLIENT
    using Components::VoxelComponent;
    VoxelComponent* voxel = (VoxelComponent*)object->get_component(COMPONENT_TEXTURED_VOXEL);
    voxel->delta_rotation();
    #endif    
    
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

//void update_medium_rock_block_drop(Object* object);


} // Objects
