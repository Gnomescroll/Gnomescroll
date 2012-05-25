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
}

void CreatePacketOwnerTeam::message(Object* object, object_create_owner_team_StoC* msg)
{
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::TeamComponent;
    
    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    msg->owner = owner->get_owner();

    TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    msg->team = team->get_team();
}


void CreatePacketOwnerTeamIndex::message(Object* object, object_create_owner_team_index_StoC* msg)
{
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::IndexedTeamComponent;

    msg->id = object->id;
    msg->type = object->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    msg->owner = owner->get_owner();

    IndexedTeamComponent* team = (IndexedTeamComponent*)object->get_component(COMPONENT_INDEXED_TEAM);
    msg->team = team->get_team();
    msg->team_index = team->get_team_index();
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
    GS_ASSERT(health->health == health->max_health); // assert this, because our create packet handler assumes they are the same
    GS_ASSERT(health->max_health >= 0);
    msg->health = health->max_health;
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
}



} // Objects
