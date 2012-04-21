#pragma once

#include <c_lib/entity/component/component.hpp>

namespace Components
{

class MotionTargetingComponent: public TargetingComponent
{
    public:

        Vec3 lock_target(Vec3 camera_position);
        Vec3 lock_target(Vec3 camera_position, int team);

    MotionTargetingComponent()
    : TargetingComponent(COMPONENT_MOTION_TARGETING)
    {}
};

} // Components
