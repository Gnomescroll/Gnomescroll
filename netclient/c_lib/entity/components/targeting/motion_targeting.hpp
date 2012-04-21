#pragma once

#include <c_lib/entity/component/component.hpp>

namespace Components
{

class MotionTargetingComponent: public TargetingComponent
{
    public:

        Vec3 destination;
        bool at_destination;
        bool en_route;
        int ticks_to_destination;
        float speed;

        void lock_target(Vec3 camera_position);
        void lock_target(Vec3 camera_position, int team);

        void orient_to_target(Vec3 camera_position);

    MotionTargetingComponent()
    : TargetingComponent(COMPONENT_MOTION_TARGETING)
    {}
};

} // Components
