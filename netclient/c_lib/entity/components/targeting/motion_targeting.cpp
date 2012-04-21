#include "motion_targeting.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/ray_trace/handlers.hpp>

namespace Components
{

/* Targeting */

Vec3 MotionTargetingComponent::lock_target(Vec3 camera_position, int team)
{   // lock on agent
    Vec3 direction;
    this->target = Hitscan::lock_agent_target(
        camera_position, &direction, team,
        this->sight_range, this->target_acquisition_probability,
        true, false
    );
    if (this->target != NULL) this->locked_on_target = true;
    return direction;
}

Vec3 MotionTargetingComponent::lock_target(Vec3 camera_position)
{
    return this->lock_target(camera_position, NO_TEAM);
}

} // Objects
