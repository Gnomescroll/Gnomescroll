#include "medium_rock.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/verlet.hpp>
#include <c_lib/entity/components/pickup.hpp>
#if DC_CLIENT
#include <c_lib/entity/components/draw/voxel.hpp>
#include <c_lib/t_map/glsl/texture.hpp>
#endif

namespace Objects
{

void load_medium_rock_block_drop_data()
{
    load_block_drop_data(OBJECT_MEDIUM_ROCK_BLOCK_DROP);
}

static void set_medium_rock_block_drop_properties(Object* object)
{
    #if DC_CLIENT
    using Components::TexturedVoxelComponent;
    TexturedVoxelComponent* voxel = (TexturedVoxelComponent*)add_component_to_object(object, COMPONENT_TEXTURED_VOXEL);
    voxel->texture_index = t_map::get_cube_primary_texture_index((char*)"medium_rock");   // TODO -- use cached values
    voxel->pixel_width = BLOCK_DROP_TEXTURE_PIXEL_WIDTH;
    voxel->size = BLOCK_DROP_SIZE;
    voxel->dtheta_speed = BLOCK_DROP_THETA_ROTATION_SPEED;
    voxel->dphi_speed = BLOCK_DROP_PHI_ROTATION_SPEED;
    #endif
    
    using Components::VerletPhysicsComponent;
    using Components::PickupComponent;
    using Components::TTLHealthComponent;
    
    VerletPhysicsComponent* physics = (VerletPhysicsComponent*)add_component_to_object(object, COMPONENT_VERLET);
    physics->mass = BLOCK_DROP_MASS;
    physics->damp = BLOCK_DROP_DAMP;
    
    PickupComponent* pickup = (PickupComponent*)add_component_to_object(object, COMPONENT_PICKUP);
    pickup->pickup_radius = BLOCK_DROP_PICKUP_RADIUS;
    
    TTLHealthComponent* health = (TTLHealthComponent*)add_component_to_object(object, COMPONENT_TTL);
    health->ttl_max = BLOCK_DROP_TTL;

    object->tick = &tick_medium_rock_block_drop;

    object->create = create_packet_momentum;
    object->state = state_packet_momentum;
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
    object->broadcastCreate();
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
    pickup->tick();
    #endif

    TTLHealthComponent* ttl = (TTLHealthComponent*)object->get_component(COMPONENT_TTL);
    ttl->tick();
}

//void update_medium_rock_block_drop(Object* object);


} // Objects
