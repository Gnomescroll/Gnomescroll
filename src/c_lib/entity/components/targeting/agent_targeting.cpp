#include "motion_targeting.hpp"

#include <physics/vec3.hpp>
#include <physics/motion.hpp>
#include <physics/verlet.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <entity/network/packets.hpp>
#include <common/random.hpp>
#include <agent/_interface.hpp>

namespace Components
{

/* Targeting */

void AgentTargetingComponent::set_target(AgentID agent_id)
{
    Agents::Agent* a = Agents::get_agent((AgentID)agent_id);
    IF_ASSERT(a == NULL) return;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;
    Vec3 position = physics->get_position();

    Vec3 dest = a->get_position();
    this->target_direction = quadrant_translate_position(position, dest);
    this->target_direction = vec3_normalize(this->target_direction);

    this->target_type = OBJECT_AGENT;
    this->target_id = agent_id;
    this->locked_on_target = true;
}

void AgentTargetingComponent::check_target_alive()
{
    if (this->target_type != OBJECT_AGENT) return;
    Agents::Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL || target->status.dead)
    {
        this->target_id = NULL_AGENT;
        this->target_type = OBJECT_NONE;
    }
}

void AgentTargetingComponent::lock_target(Vec3 camera_position)
{
    class Agents::Agent* target = Hitscan::lock_agent_target(
        camera_position, &this->target_direction, this->sight_range);
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
    if (this->target_type == OBJECT_NONE)
    {
        this->target_direction = vec3_init(1,0,0);
        return;
    }
    if (this->target_type != OBJECT_AGENT)
    {
        this->target_direction = vec3_init(1,0,0);
        return;
    }

    Agents::Agent* target = Agents::get_agent((AgentID)this->target_id);
    if (target == NULL)
    {
        this->target_direction = vec3_init(1,0,0);
        return;
    }

    Vec3 target_position = target->get_position();
    GS_ASSERT(is_boxed_position(target_position));
    target_position = quadrant_translate_position(camera_position, target_position);
    this->target_direction = vec3_sub(target_position, camera_position);
    this->target_direction = vec3_normalize(this->target_direction);
}

void AgentTargetingComponent::move_on_surface()
{   // adjusts position & momentum by moving over the terrain surface
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;

    // TODO -- get box dimension from dimensions component
    BoundingBox box;
    box.height = 0.8f;
    box.radius = 0.4f;

    // adjust position/momentum by moving along terrain surface
    Vec3 position = physics->get_position();
    Vec3 momentum = physics->get_momentum();
    float ground_distance = 0.0f;

    float speed = this->speed / PHYSICS_TICK_RATE;
    float base_speed = speed;
    if (this->get_target_distance(physics->get_position()) <
        this->proximity_radius * this->proximity_radius)
    {
        speed = 0.0f;
        this->jump_cooldown_max = this->jump_cooldown_nearby;
    }
    else
        this->jump_cooldown_max = this->jump_cooldown_en_route;

    CSKey jump = CS_NULL;
    if (on_ground(box.radius, position))
    {
        if (jump_cooldown_tick <= 0)
        {
            jump = CS_JUMP;
            speed = base_speed;
            this->jump_cooldown_tick = this->jump_cooldown_max;
        }
    }
    else
    {
        speed = base_speed;
    }

    Agents::Agent* collided_agent = NULL;
    bool passed_through = move_with_terrain_collision(box, position, momentum,
                                                      ground_distance,
                                                      &collided_agent);
    if (passed_through)
    {
        ControlState cs;
        cs.seq = 0;  // doesn't matter, but initialise it
        vec3_to_angles(this->target_direction, &cs.theta, &cs.phi);
        cs.cs = 0;
        cs.cs |= CS_FORWARD;
        cs.cs |= jump;
        #if DC_SERVER
        if (jump)
        {
            const char* sound_name = Sound::get_random_name("slime_jump_%d", 1, 3);
            Sound::broadcast_play_3d_sound(sound_name, position);
        }
        #endif
        apply_control_state(cs, speed, this->jump_force, position, momentum,
                            ground_distance);
    }

    #if DC_SERVER
    if (collided_agent != NULL && attack_tick <= 0)
    {
        collided_agent->status.apply_damage(this->attack_damage, this->object->type);
        this->attack_tick = this->attack_rate;
    }
    #endif

    physics->set_position(position);
    physics->set_momentum(momentum);
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
        this->target_id = NULL_AGENT;
        this->ticks_locked = 0;

        using Components::StateMachineComponent;
        StateMachineComponent* state_machine = (StateMachineComponent*)
            this->object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
        if (state_machine != NULL && state_machine->router != NULL)
            state_machine->router(this->object, STATE_WAITING);
    }
    this->jump_cooldown_tick = GS_MAX(this->jump_cooldown_tick - 1, 0);
    this->attack_tick = GS_MAX(this->attack_tick - 1, 0);
}

} // Entities
