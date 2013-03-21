#include "interfaces.hpp"

#include <entity/object/object.hpp>
#include <entity/components/physics.hpp>

namespace Entities
{

/* Create */

void CreatePacket::message(Entity* object, object_create_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    msg->position = physics->get_position();
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketOwner::message(Entity* object, object_create_owner_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    msg->position = physics->get_position();
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    IF_ASSERT(owner == NULL)
        msg->owner = NULL_AGENT;
    else
        msg->owner = owner->get_owner();
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentum::message(Entity* object, object_create_momentum_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentumAngles::message(Entity* object, object_create_momentum_angles_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentumAnglesHealth::message(Entity* object, object_create_momentum_angles_health_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    using Components::HitPointsHealthComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)object->get_component(COMPONENT_HIT_POINTS);
    IF_ASSERT(health == NULL || health->health_max < 0)
        msg->health_max = 0; // should be dead
    else
        msg->health_max = health->health_max;
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentumAnglesHealth::health_message(Entity* object, object_state_health_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)object->get_component(COMPONENT_HIT_POINTS);
    IF_ASSERT(health == NULL || health->health < 0)
        msg->health = 0;
    else
        msg->health = health->health;
}


/* State */

void StatePacket::message(Entity* object, object_state_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    msg->position = physics->get_position();
    GS_ASSERT(is_boxed_position(msg->position));
}

void StatePacketMomentum::message(Entity* object, object_state_momentum_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    GS_ASSERT(is_boxed_position(msg->position));
}

void StatePacketMomentumAngles::message(Entity* object, object_state_momentum_angles_StoC* msg)
{
    msg->id = object->id;
    msg->type = object->type;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;
    GS_ASSERT(is_boxed_position(msg->position));
}

} // Entities
