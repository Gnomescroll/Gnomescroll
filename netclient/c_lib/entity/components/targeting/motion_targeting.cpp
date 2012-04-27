#include "motion_targeting.hpp"

#include <physics/vec3.hpp>
#include <physics/motion.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/handlers.hpp>
#include <entity/network/packets.hpp>

namespace Components
{

/* Targeting */

void MotionTargetingComponent::lock_target(Vec3 camera_position, int team)
{   // lock on agent
    Agent_state* target;
    target = Hitscan::lock_agent_target(
        camera_position, &this->target_direction, team,
        this->sight_range, this->target_acquisition_probability,
        true, false
    );
    if (target == NULL)
    {
        this->target_type = OBJECT_NONE;
        return;
    }
    this->target_type = OBJECT_AGENT;
    this->target_id = target->id;
}

void MotionTargetingComponent::lock_target(Vec3 camera_position)
{
    Agent_state* target;
    target = Hitscan::lock_agent_target(camera_position, &this->target_direction, this->sight_range);
    if (target == NULL)
    {
        this->target_type = OBJECT_NONE;
        return;
    }
    this->target_type = OBJECT_AGENT;
    this->target_id = target->id;
}

void MotionTargetingComponent::orient_to_target(Vec3 camera_position)
{
    if (this->target_type == OBJECT_NONE) return;
    if (this->target_type != OBJECT_AGENT) return;  //  todo -- target all types
    Agent_state* target = STATE::agent_list->get(this->target_id);
    if (target == NULL) return;
    Vec3 target_position = vec3_init(target->s.x, target->s.y, target->s.z);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

//void MotionTargetingComponent::lock_target_destination(Vec3 camera_position)
//{
    
//}

// adjusts position & momentum by moving over the terrain surface
bool MotionTargetingComponent::move_on_surface()
{
    // get physics data
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    assert(physics != NULL);

    // adjust position/momentum by moving along terrain surface
    Vec3 new_position;
    Vec3 new_momentum;
    Vec3 motion_direction = vec3_init(this->target_direction.x, this->target_direction.y, 0);
    bool moved = move_along_terrain_surface(
        physics->get_position(), motion_direction, this->speed,
        this->max_z_down, this->max_z_up,
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


void MotionTargetingComponent::broadcast_destination()
{
    object_choose_destination_StoC msg;
    msg.x = this->destination.x;
    msg.y = this->destination.y;
    msg.z = this->destination.z;
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.ticks = this->ticks_to_destination;
    msg.broadcast();
}

} // Objects
