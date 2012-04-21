#pragma once

#include <c_lib/entity/component/component.hpp>

// forward decl
class Agent_state;

namespace Components
{

class TargetingComponent: public Component
{
    public:
        // data
        float target_acquisition_probability;
        float sight_range;

        // state
        Agent_state* target;
        Vec3 target_direction;
        bool locked_on_target;

        virtual Vec3 lock_target(Vec3 camera_position, int team) = 0;
        virtual Vec3 lock_target(Vec3 camera_position) = 0;

    explicit TargetingComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_TARGETING),
    target_acquisition_probability(1.0f), sight_range(10.0f),
    target(NULL), locked_on_target(false)
    {}
};

} // Components
