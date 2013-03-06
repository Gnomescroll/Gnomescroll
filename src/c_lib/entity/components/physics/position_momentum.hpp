#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionMomentumPhysicsComponent: public PhysicsComponent
{
    public:
        Vec3 position;
        Vec3 momentum;
        Vec3 angles;

    Vec3 get_position()
    {
        return this->position;
    }

    bool set_position(Vec3 position)
    {
        this->position = translate_position(position);
        return true;
    }

    Vec3 get_momentum()
    {
        return this->momentum;
    }

    bool set_momentum(Vec3 momentum)
    {
        this->momentum = momentum;
        return true;
    }

    Vec3 get_angles()
    {
        return this->angles;
    }

    bool set_angles(Vec3 angles)
    {
        this->angles = angles;
        return true;
    }

    PositionMomentumPhysicsComponent() :
        PhysicsComponent(COMPONENT_POSITION_MOMENTUM),
        position(NULL_POSITION), momentum(NULL_MOMENTUM), angles(NULL_ANGLES)
    {}
};



} // Components
