#include "motion_targeting.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/ray_trace/handlers.hpp>
#include <c_lib/entity/network/packets.hpp>

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
    this->lock_target(camera_position, NO_TEAM);
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

void MotionTargetingComponent::lock_target_destination(Vec3 camera_position)
{
    
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
