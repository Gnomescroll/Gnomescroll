#include "motion_targeting.hpp"

#include <physics/vec3.hpp>
#include <physics/motion.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <entity/network/packets.hpp>
#include <common/random.hpp>
#include <agent/_interface.hpp>

namespace Components
{

/* Targeting */

void MotionTargetingComponent::set_target(EntityType target_type, int target_id)
{
    GS_ASSERT(target_type == OBJECT_AGENT);
    if (target_type != OBJECT_AGENT) return;

    Agent* a = Agents::get_agent((AgentID)target_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    Vec3 dest = a->get_position();
    this->target_direction = quadrant_translate_position(position, dest);
    normalize_vector(&this->target_direction);
    
    this->target_type = target_type;
    this->target_id = target_id;
    this->locked_on_target = true;

    #if DC_SERVER
    this->broadcast_target_choice();
    #endif
}

void MotionTargetingComponent::check_target_alive()
{
    if (this->target_type != OBJECT_AGENT) return;
    Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL || target->status.dead)
    {
        this->target_id = NULL_AGENT;
        this->target_type = OBJECT_NONE;
        #if DC_SERVER
        this->broadcast_remove_target();
        #endif
    }
}

void MotionTargetingComponent::lock_target(Vec3 camera_position)
{
    Agent* target;
    target = Hitscan::lock_agent_target(camera_position, &this->target_direction, this->sight_range);
    if (target == NULL)
    {
        this->target_type = OBJECT_NONE;
        return;
    }
    this->target_type = OBJECT_AGENT;
    this->target_id = target->id;
    #if DC_SERVER
    this->broadcast_target_choice();
    #endif
}

void MotionTargetingComponent::choose_destination()
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
}

void MotionTargetingComponent::orient_to_target(Vec3 camera_position)
{
    if (this->target_type == OBJECT_NONE) return;
    if (this->target_type != OBJECT_AGENT) return;  //  todo -- target all types
    Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL) return;
    Vec3 target_position = target->get_position();
    target_position = quadrant_translate_position(camera_position, target_position);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

// adjusts position & momentum by moving over the terrain surface
bool MotionTargetingComponent::move_on_surface()
{
    // get physics data
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;
    Vec3 motion_direction = vec3_init(this->target_direction.x, this->target_direction.y, 0);
    bool moved = move_along_terrain_surface(
        physics->get_position(), motion_direction,
        this->speed, this->max_z_diff,
        &new_position, &new_momentum
    );
    physics->set_position(new_position);
    physics->set_momentum(new_momentum);

    new_momentum.z = 0;
    if (vec3_length_squared(new_momentum))
    {   // update target direction
        normalize_vector(&new_momentum);
        this->target_direction = new_momentum;
    }

    // set en_route if we are in motion
    this->en_route = moved;
    
    return moved;
}

#if DC_SERVER
void MotionTargetingComponent::broadcast_target_choice()
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

void MotionTargetingComponent::broadcast_remove_target()
{
    GS_ASSERT(this->object != NULL);
    if (this->object == NULL) return;
    object_remove_motion_target_StoC msg;
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.broadcast();
}

void MotionTargetingComponent::broadcast_destination()
{
    object_choose_destination_StoC msg;
    msg.x = this->destination.x;
    msg.y = this->destination.y;
    msg.z = this->destination.z;
    ASSERT_BOXED_POSITION(this->destination);
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.ticks_to_destination = this->ticks_to_destination;
    msg.broadcast();
}
#endif

void MotionTargetingComponent::call()
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
        this->target_id = NULL_AGENT;
        this->ticks_locked = 0;
        #if DC_SERVER
        this->broadcast_remove_target();
        #endif
    }
}

} // Entities
