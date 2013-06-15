#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class TargetingComponent: public Component
{
    public:
        // data
        float target_acquisition_failure_rate;
        float sight_range;

        // state
        int target_id;
        EntityType target_type;
        Vec3 target_direction;
        bool locked_on_target;

    virtual ~TargetingComponent() {}

    explicit TargetingComponent(ComponentType type) :
        Component(type, COMPONENT_INTERFACE_TARGETING),
        target_acquisition_failure_rate(0.0f), sight_range(10.0f),
        target_id(-1), target_type(NULL_ENTITY_TYPE), locked_on_target(false)
    {}
};

} // Components
