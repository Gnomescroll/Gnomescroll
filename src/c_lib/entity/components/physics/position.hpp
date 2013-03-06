#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionPhysicsComponent: public PhysicsComponent
{
    public:
        Vec3 position;
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
        return NULL_MOMENTUM;
    }

    bool set_momentum(Vec3 momentum)
    {
        return false;
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

    PositionPhysicsComponent() :
        PhysicsComponent(COMPONENT_POSITION),
        position(NULL_POSITION), angles(NULL_ANGLES)
    {}
};


} // Components
