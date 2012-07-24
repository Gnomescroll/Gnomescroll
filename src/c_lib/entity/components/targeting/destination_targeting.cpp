#include "destination_targeting.hpp"

#include <physics/vec3.hpp>
#include <physics/motion.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <entity/network/packets.hpp>
#include <common/random.h>

namespace Components
{

/* Targeting */

void DestinationTargetingComponent::choose_destination()
{
    float x = (randf()-0.5f)*2 * this->destination_choice_x;
    float y = (randf()-0.5f)*2 * this->destination_choice_y;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;

    Vec3 position = physics->get_position();
    position.x += x;
    position.y += y;
    position.z = t_map::get_highest_solid_block(position.x, position.y);
    this->destination = translate_position(position);
    this->at_destination = false;
    this->en_route = true;
    this->ticks_to_destination = vec3_length(vec3_init(x,y,0)) / this->speed;
}

void DestinationTargetingComponent::orient_to_target(Vec3 camera_position)
{
    Vec3 target_position = quadrant_translate_position(camera_position, this->destination);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

// adjusts position & momentum by moving over the terrain surface
bool DestinationTargetingComponent::move_on_surface()
{
    // get physics data
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;

    bool moved = move_along_terrain_surface(
        physics->get_position(), this->target_direction,
        this->speed, this->max_z_diff,
        &new_position, &new_momentum
    );
    physics->set_position(new_position);
    physics->set_momentum(new_momentum);

    if (vec3_length(new_momentum))
    {   // update target direction
        new_momentum.z = 0;
        normalize_vector(&new_momentum);
        this->target_direction = new_momentum;
    }

    // set en_route if we are in motion
    this->en_route = moved;
    
    return moved;
}

void DestinationTargetingComponent::check_at_destination()
{
    if (!this->en_route)
    {   // TODO -- remove this hack. says we are at destination if we are not moving
        this->at_destination = true;
        return;
    }
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL)
    {
        this->at_destination = false;
        return;
    }
    Vec3 pos = physics->get_position();
    Vec3 dest = quadrant_translate_position(pos, this->destination);
    if (vec3_distance_squared(pos, dest) <= this->stop_proximity)
        this->at_destination = true;
    else
        this->at_destination = false;
}

void DestinationTargetingComponent::broadcast_target_choice()
{
    GS_ASSERT(this->object != NULL);
    if (this->object == NULL) return;
    object_choose_motion_target_StoC msg;
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.target_id = this->target_id;
    msg.target_type = this->target_type;
    msg.broadcast();
}

void DestinationTargetingComponent::broadcast_remove_target()
{
    GS_ASSERT(this->object != NULL);
    if (this->object == NULL) return;
    object_remove_motion_target_StoC msg;
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.broadcast();
}

void DestinationTargetingComponent::broadcast_destination()
{
    object_choose_destination_StoC msg;
    msg.x = this->destination.x;
    msg.y = this->destination.y;
    msg.z = this->destination.z;
    ASSERT_BOXED_POSITION(this->destination);
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.ticks = this->ticks_to_destination;
    msg.broadcast();
}

void DestinationTargetingComponent::call()
{
    if (this->target_type == OBJECT_NONE)
    {
        this->ticks_locked = 0;
        return;
    }
    this->ticks_locked++;
    if (this->max_lock_ticks && this->ticks_locked > this->max_lock_ticks)
    {   // reset
        this->target_type = OBJECT_NONE;
        this->target_id = NO_AGENT;
        this->ticks_locked = 0;
        //this->broadcast_remove_target();
    }
}

} // Objects
