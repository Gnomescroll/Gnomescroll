#include "bomb.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>

namespace Objects
{

static void set_mob_bomb_properties(Object* object)
{
    const int n_components = 4;
    object->init(n_components);
    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM_CHANGED);
    add_component_to_object(object, COMPONENT_DIMENSION);
    add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    add_component_to_object(object, COMPONENT_HIT_POINTS);

    object->tick = &tick_mob_bomb;
    object->update = &update_mob_bomb;

    object->create = create_packet_momentum_angles;
    object->state = state_packet_momentum_angles;
}

Object* create_mob_bomb()
{
    // initialize object
    ObjectType type = OBJECT_MONSTER_BOMB;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_bomb_properties(obj);
    return obj;
}

void ready_mob_bomb(Object* object)
{
    using Components::VoxelModelComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_mob_bomb(Object* object)
{
    #if DC_CLIENT
    // voxel animation
    #endif

    #if DC_SERVER
    // drop item
    // explosion damage
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    explode->explode();
    object->broadcastDeath();
    #endif

}

void tick_mob_bomb(Object* object)
{
    //using Components::PhysicsComponent;

    //PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    //Vec3 position = physics->get_position();
    //Agent_state* agent;
    
    #if DC_SERVER
     //die if near agent
     //TODO -- proximity effect component
    //agent = nearest_agent_in_range(position, this->_state.explosion_proximity_radius);
    //if (agent != NULL)
    //{
        //this->health.properties.dead = true;
        //return;
    //}
    #endif
    
    //// acquire target
    //agent = nearest_agent_in_range(position, this->_state.motion_proximity_radius);
    //if (agent == NULL) return;
    //Vec3 agent_position = vec3_init(agent->s.x, agent->s.y, agent->s.z);

    //// face the target
    //Vec3 angles = physics->get_angles();
    //angles.x = tickOrientToPointTheta(agent_position, position); // only rotate in x
    //this->set_angles(angles);

    //// move towards target
    //position = tickMoveToPoint(agent_position, position, physics->get_momentum());       // vector between agent and slime
    //physics->set_position(position); // move slime position by velocity

    #if DC_SERVER
    // TODO -- rate limited broadcast component
    //if (this->canSendState())
    object->broadcastState(); // send state packet every N ticks
    #endif

}

void update_mob_bomb(Object* object)
{
    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}


} // Objects

