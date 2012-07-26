#include "motion_targeting.hpp"

#include <physics/vec3.hpp>
#include <physics/motion.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <entity/network/packets.hpp>
#include <common/random.h>

namespace Components
{

/* Targeting */

void AgentTargetingComponent::set_target(int agent_id)
{
    Agent_state* a = STATE::agent_list->get(agent_id);
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
    
    this->target_type = OBJECT_AGENT;
    this->target_id = agent_id;
    this->locked_on_target = true;
}

void AgentTargetingComponent::check_target_alive()
{
    if (this->target_type != OBJECT_AGENT) return;
    Agent_state* target = STATE::agent_list->get(this->target_id);
    if (target == NULL || target->status.dead)
    {
        this->target_id = NO_AGENT;
        this->target_type = OBJECT_NONE;
    }
}

void AgentTargetingComponent::lock_target(Vec3 camera_position)
{
    class Agent_state* target = Hitscan::lock_agent_target
        (camera_position, &this->target_direction, this->sight_range);
    if (target == NULL)
    {
        this->target_type = OBJECT_NONE;
        return;
    }
    this->target_type = OBJECT_AGENT;
    this->target_id = target->id;
}

void AgentTargetingComponent::orient_to_target(Vec3 camera_position)
{
    if (this->target_type == OBJECT_NONE) return;
    if (this->target_type != OBJECT_AGENT) return;  //  todo -- target all types
    Agent_state* target = STATE::agent_list->get(this->target_id);
    if (target == NULL) return;
    Vec3 target_position = target->get_position();
    target_position = quadrant_translate_position(camera_position, target_position);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

// adjusts position & momentum by moving over the terrain surface
bool AgentTargetingComponent::move_on_surface()
{
    // get physics data
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;
    Vec3 motion_direction = this->target_direction;
    motion_direction.z = 0.0f;

    normalize_vector(&motion_direction);

    bool moved = move_along_terrain_surface(
        physics->get_position(), motion_direction,
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

    return moved;
}

void AgentTargetingComponent::call()
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

        using Components::StateMachineComponent;
        StateMachineComponent* state_machine = (StateMachineComponent*)
            this->object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
        if (state_machine != NULL && state_machine->router != NULL)
            state_machine->router(this->object, STATE_WAITING);
    }
}

} // Objects
