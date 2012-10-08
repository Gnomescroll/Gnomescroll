#include "interfaces.hpp"

#include <entity/object/object.hpp>
#include <entity/components/physics.hpp>

namespace Objects
{
    
/* Create */

void CreatePacket::message(Object* object, object_create_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    ASSERT_BOXED_POSITION(position);
}

void CreatePacketOwner::message(Object* object, object_create_owner_StoC* msg)
{
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    GS_ASSERT(owner != NULL);
    if (owner != NULL)
        msg->owner = owner->get_owner();
    else msg->owner = NULL_AGENT;

    ASSERT_BOXED_POSITION(position);
}

void CreatePacketMomentum::message(Object* object, object_create_momentum_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    Vec3 momentum = physics->get_momentum();
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;

    ASSERT_BOXED_POSITION(position);
}

void CreatePacketMomentumAngles::message(Object* object, object_create_momentum_angles_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    Vec3 momentum = physics->get_momentum();
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;

    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;
    
    ASSERT_BOXED_POSITION(position);
}

void CreatePacketMomentumAnglesHealth::message(Object* object, object_create_momentum_angles_health_StoC* msg)
{
    using Components::PhysicsComponent;
    using Components::HitPointsHealthComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    Vec3 momentum = physics->get_momentum();
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;

    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;

    HitPointsHealthComponent* health = (HitPointsHealthComponent*)object->get_component(COMPONENT_HIT_POINTS);
    GS_ASSERT(health != NULL);
    if (health == NULL) return;
    GS_ASSERT(health->max_health >= 0); // should be dead
    msg->max_health = health->max_health;

    ASSERT_BOXED_POSITION(position);
}

void CreatePacketMomentumAnglesHealth::health_message(Object* object, object_state_health_StoC* msg)
{
    GS_ASSERT(msg != NULL);

    using Components::HitPointsHealthComponent;

    msg->id = object->id;
    msg->type = object->type;

    HitPointsHealthComponent* health = (HitPointsHealthComponent*)object->get_component(COMPONENT_HIT_POINTS);
    GS_ASSERT(health != NULL);
    if (health == NULL) return;
    GS_ASSERT(health->health >= 0);
    msg->health = health->health;
}


/* State */

void StatePacket::message(Object* object, object_state_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    ASSERT_BOXED_POSITION(position);
}

void StatePacketMomentum::message(Object* object, object_state_momentum_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    Vec3 momentum = physics->get_momentum();
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;

    ASSERT_BOXED_POSITION(position);
}

void StatePacketMomentumAngles::message(Object* object, object_state_momentum_angles_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    Vec3 momentum = physics->get_momentum();
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;

    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;

    ASSERT_BOXED_POSITION(position);
}



} // Objects
