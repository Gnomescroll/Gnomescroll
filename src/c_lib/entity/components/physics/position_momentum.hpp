#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionMomentumPhysicsComponent: public PositionPhysicsComponent
{
    protected:
        Vec3 momentum;

    public:

    Vec3 get_momentum()
    {
        return this->momentum;
    }

    bool set_momentum(Vec3 momentum)
    {
        IF_ASSERT(!vec3_is_valid(momentum))
            return false;
        if (vec3_equal(this->momentum, momentum))
            return false;
        this->momentum = momentum;
        this->changed = true;
        return this->changed;
    }

    PositionMomentumPhysicsComponent() :
        PositionPhysicsComponent(COMPONENT_POSITION_MOMENTUM),
        momentum(NULL_MOMENTUM)
    {}

};

} // Components
