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
    class Agents::Agent* target = Hitscan::lock_agent_target
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


//void move(Vec3& position, Vec3& velocity, const BoundingBox& box)
//{   // position, momentum are both inputs and outputs
    //// NOTE: assumes orientation in normalized
    ////Vec3 forward = vec3_init(0);
    ////if (on_ground(())   // TODO -- in_air/on_ground check

    ////bool current_collision = collision_check_current(box.radius, box.height, position);
    ////if (current_collision)
    ////{
        ////position.z += 0.02f;    // nudge up
        ////velocity.x = vec3_init(0, 0, GS_MAX(velocity.z, 0));
        ////return;
    ////}

    ////Vec3 new_pos = translate_position(vec3_add(position, velocity));
    ////bool cx = collision_check_xy(box.radius, box.height, new_pos.x, position.y, position.z);
    ////if (cx) new_pos.x = position.x;
    ////bool cy = collision_check_xy(box.radius, box.height, position.x, new_pos.y, position.z);
    ////if (cy) new_pos.y = position.y;

    ////float solid_z = clamp_to_ground(box.radius, new_pos.x, new_pos.y, position.z);
    ////bool top = false;
    ////bool cz = collision_check_z(box.radius, height, new_pos.x, new_pos.y, new_pos.z, &top);
    ////if (cz)
    ////{
        ////if (top && velocity.z > 0)
            ////new_pos.z = floorf(position.z) + ceilf(box.height) - box.height;
        ////else
            ////new_pos.z = solid_z;
        ////velocity.z = 0.0f;
    ////}

    ////new_pos.z = GS_MAX(new_pos.z, solid_z);
    ////if (new_pos.z == solid_z) velocity.z = 0.0f;
    ////float ground_distance = new_z - solid_z;

    ////Vec3i new_vel = vec3_init(0);
    ////if (forward)
//}


//Vec3 jump(const Vec3& momentum, float force, float weight)
//{
    //IF_ASSERT(weight <= 0.0f) return momentum;
    //return vec3_add(momentum, vec3_init(0, 0, force / weight));
//}

//Vec3 knockback(const Vec3& momentum, const Vec3& incident,
               //ItemType item, float weight)
//{   // TODO -- lookup force for item
    //// Note: assumes incident is normalized
    //IF_ASSERT(weight <= 0.0f) return momentum;
    //static const float ITEM_FORCE = 4.0f;
    //float force = ITEM_FORCE / weight;
    //Vec3 force_vector = vec3_scalar_mult(incident, force);
    //return vec3_add(momentum, force_vector);
//}

void AgentTargetingComponent::move_on_surface()
{   // adjusts position & momentum by moving over the terrain surface
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;

    // adjust position/momentum by moving along terrain surface
    Vec3 position = physics->get_position();
    Vec3 momentum = physics->get_momentum();
    float ground_distance;

    float speed = this->speed / PHYSICS_TICK_RATE;
    if (this->get_target_distance(physics->get_position()) <
        this->proximity_radius * this->proximity_radius)
    {
            speed = 0.0f;
    }

    // TODO -- get box dimension from dimensions component
    BoundingBox box;
    box.height = 0.8f;
    box.radius = 0.4f;
    bool passed_through = move_with_collision(box, position, momentum, ground_distance);
    if (passed_through)
    {
        ControlState cs;
        cs.seq = 0; // doesn't matter, but initialise it
        vec3_to_angles(this->target_direction, &cs.theta, &cs.phi);
        cs.cs = 0;
        cs.cs |= CS_FORWARD;
        apply_control_state(cs, speed, position, momentum, ground_distance);
    }

    //move_within_terrain_surface(physics->get_position(), motion_direction,
                                //this->speed, this->max_z_diff,
                                //&position, &momentum);
    physics->set_position(position);
    physics->set_momentum(momentum);

    momentum.z = 0;
    if (vec3_length_squared(momentum))
    {   // update target direction
        momentum = vec3_normalize(momentum);
        this->target_direction = momentum;
    }
}


/*
 * Physics:
 *
 *  move():
 *      Inputs: position, momentum, orientation, speed
 *          -- assumes forward movement (so no fblr)
 *      Outputs: position, momentum
 *
 *  jump():
 *      Inputs: momentum, force, weight
 *      Outputs: momentum
 *
 *  knockback():
 *      Inputs: momentum, incident, knockback_item, object_weight
 *      Outputs: momentum
 *
 *  jump, knockback just update the momentum vector
 *
 *  move() uses the momentum vector to calculate position.
 *  it also does collision detection
 *  friction force will converge the momentum vector to the speed vector
 *  if in air, can't move forward on its own (but can be knocked back)
 *
 */

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
}

} // Entities
