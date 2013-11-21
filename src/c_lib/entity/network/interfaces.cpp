/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "interfaces.hpp"

#include <entity/entity/entity.hpp>
#include <entity/components/physics.hpp>

namespace Entities
{

/* Create */

void CreatePacket::message(const Entity* entity, entity_create_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    msg->position = physics->get_position();
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketOwner::message(const Entity* entity, entity_create_owner_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    msg->position = physics->get_position();
    auto owner = GET_COMPONENT_INTERFACE(Owner, entity);
    IF_ASSERT(owner == NULL)
        msg->owner = NULL_AGENT;
    else
        msg->owner = owner->get_owner();
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentum::message(const Entity* entity, entity_create_momentum_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentumAngles::message(const Entity* entity, entity_create_momentum_angles_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentumAnglesHealth::message(const Entity* entity, entity_create_momentum_angles_health_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    IF_ASSERT(physics == NULL) return;
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;
    auto health = GET_COMPONENT(HitPoints, entity);
    IF_ASSERT(health == NULL || health->health_max < 0)
        msg->health_max = 0; // should be dead
    else
        msg->health_max = health->health_max;
    GS_ASSERT(is_boxed_position(msg->position));
}

void CreatePacketMomentumAnglesHealth::health_message(const Entity* entity, entity_state_health_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto health = GET_COMPONENT(HitPoints, entity);
    IF_ASSERT(health == NULL || health->health < 0)
        msg->health = 0;
    else
        msg->health = health->health;
}

/* State */

void StatePacket::message(const Entity* entity, entity_state_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    msg->position = physics->get_position();
    GS_ASSERT(is_boxed_position(msg->position));
}

void StatePacketMomentum::message(const Entity* entity, entity_state_momentum_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    GS_ASSERT(is_boxed_position(msg->position));
}

void StatePacketMomentumAngles::message(const Entity* entity, entity_state_momentum_angles_StoC* msg) const
{
    msg->id = entity->id;
    msg->type = entity->type;
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    msg->position = physics->get_position();
    msg->momentum = physics->get_momentum();
    Vec3 angles = physics->get_angles();
    msg->theta = angles.x;
    msg->phi = angles.y;
    GS_ASSERT(is_boxed_position(msg->position));
}

} // Entities
