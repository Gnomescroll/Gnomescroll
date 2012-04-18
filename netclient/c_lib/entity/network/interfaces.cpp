#include "interfaces.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/components/physics.hpp>

namespace Objects
{
    
/* Create */

void CreatePacket::message(Object* obj, object_create_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
}

void CreatePacketOwnerTeam::message(Object* obj, object_create_owner_team_StoC* msg)
{
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::TeamComponent;
    
    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    OwnerComponent* owner = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    msg->owner = owner->get_owner();

    TeamComponent* team = (TeamComponent*)obj->get_component_interface(COMPONENT_INTERFACE_TEAM);
    msg->team = team->get_team();
}


void CreatePacketOwnerTeamIndex::message(Object* obj, object_create_owner_team_index_StoC* msg)
{
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::IndexedTeamComponent;

    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    OwnerComponent* owner = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    msg->owner = owner->get_owner();

    IndexedTeamComponent* team = (IndexedTeamComponent*)obj->get_component(COMPONENT_INDEXED_TEAM);
    msg->team = team->get_team();
    msg->team_index = team->get_team_index();
}

void CreatePacketMomentum::message(Object* obj, object_create_momentum_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    Vec3 momentum = physics->get_momentum();
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;
}

void CreatePacketMomentumAngles::message(Object* obj, object_create_momentum_angles_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
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


/* State */

void StatePacket::message(Object* obj, object_state_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
}

void StatePacketMomentum::message(Object* obj, object_state_momentum_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;

    Vec3 momentum = physics->get_momentum();
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;
}

void StatePacketMomentumAngles::message(Object* obj, object_state_momentum_angles_StoC* msg)
{
    using Components::PhysicsComponent;
    
    msg->id = obj->id;
    msg->type = obj->type;
    
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
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
