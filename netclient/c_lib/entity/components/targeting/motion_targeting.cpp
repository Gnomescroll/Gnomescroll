#include "motion_targeting.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/ray_trace/handlers.hpp>

namespace Components
{

/* Targeting */

Vec3 MotionTargetingComponent::lock_target(Vec3 camera_position, int team)
{   // lock on agent
    this->target = Hitscan::lock_agent_target(
        camera_position, &this->target_direction, team,
        this->sight_range, this->target_acquisition_probability,
        true, false
    );
    if (this->target != NULL) this->locked_on_target = true;
    return this->target_direction;
}

Vec3 MotionTargetingComponent::lock_target(Vec3 camera_position)
{
    return this->lock_target(camera_position, NO_TEAM);
}

void MotionTargetingComponent::orient_to_target(Vec3 camera_position)
{
    if (this->target == NULL) return;
    Vec3 target_position = vec3_init(this->target->s.x, this->target->s.y, this->target->s.z);
    this->target_direction = vec3_sub(target_position, camera_position);
    normalize_vector(&this->target_direction);
}

} // Objects
