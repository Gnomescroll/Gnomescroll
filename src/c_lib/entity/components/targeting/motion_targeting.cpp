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
    IF_ASSERT(target_type != ENTITY_AGENT) return;

    Agents::Agent* a = Agents::get_agent((AgentID)target_id);
    IF_ASSERT(a == NULL) return;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    Vec3 dest = a->get_position();
    this->target_direction = quadrant_translate_position(position, dest);
    this->target_direction = vec3_normalize(this->target_direction);

    this->target_type = target_type;
    this->target_id = target_id;
    this->locked_on_target = true;
}

void MotionTargetingComponent::check_target_alive()
{
    if (this->target_type != ENTITY_AGENT) return;
    Agents::Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL || target->status.dead)
    {
        this->target_id = NULL_AGENT;
        this->target_type = NULL_ENTITY_TYPE;
    }
}

void MotionTargetingComponent::lock_target(Vec3 camera_position)
{
    Agents::Agent* target;
    target = Hitscan::lock_agent_target(camera_position, &this->target_direction, this->sight_range);
    if (target == NULL)
    {
        this->target_type = NULL_ENTITY_TYPE;
        return;
    }
    this->target_type = ENTITY_AGENT;
    this->target_id = target->id;
}

void MotionTargetingComponent::choose_destination()
{
    float x = 2*(randf()-0.5f) * this->destination_choice_x;
    float y = 2*(randf()-0.5f) * this->destination_choice_y;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;

    Vec3 position = physics->get_position();
    position.x += x;
    position.y += y;
    position.z = t_map::get_nearest_surface_block(position.x, position.y, position.z);
    this->destination = translate_position(position);
    this->at_destination = false;
    this->en_route = true;
}

void MotionTargetingComponent::orient_to_target(Vec3 camera_position)
{
    if (this->target_type == NULL_ENTITY_TYPE) return;
    if (this->target_type != ENTITY_AGENT) return;  //  TODO -- target all types
    Agents::Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL) return;
    Vec3 target_position = target->get_position();
    target_position = quadrant_translate_position(camera_position, target_position);
    this->target_direction = vec3_sub(target_position, camera_position);
    this->target_direction = vec3_normalize(this->target_direction);
}

// adjusts position & momentum by moving over the terrain surface
void MotionTargetingComponent::move_on_surface()
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;

    int h = 1;
    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (dims != NULL) h = dims->get_integer_height();

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;
    Vec3 motion_direction = vec3_init(this->target_direction.x, this->target_direction.y, 0);
    move_within_terrain_surface(physics->get_position(), motion_direction,
                                this->speed, this->max_z_diff,
                                &new_position, &new_momentum, h);
    physics->set_position(new_position);
    physics->set_momentum(new_momentum);

    new_momentum.z = 0;
    if (vec3_length_squared(new_momentum))
    {   // update target direction
        new_momentum = vec3_normalize(new_momentum);
        this->target_direction = new_momentum;
    }

    // set en_route if we are in motion
    this->en_route = physics->get_position_changed();
}

void MotionTargetingComponent::call()
{
    if (this->target_type == NULL_ENTITY_TYPE)
    {
        this->ticks_locked = 0;
        return;
    }
    this->ticks_locked++;
    if (this->max_lock_ticks && this->ticks_locked > this->max_lock_ticks)
    {   // reset
        this->target_type = NULL_ENTITY_TYPE;
        this->target_id = NULL_AGENT;
        this->ticks_locked = 0;
    }
}

} // Entities
