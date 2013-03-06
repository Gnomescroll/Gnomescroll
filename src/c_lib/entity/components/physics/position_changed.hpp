#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionChangedPhysicsComponent: public PhysicsComponent
{
    public:
        Vec3 position;
        Vec3 angles;
        bool changed;

    Vec3 get_position()
    {
        return this->position;
    }

    bool set_position(Vec3 position)
    {
        position = translate_position(position);
        if (vec3_equal(this->position, position)) return false;
        this->position = position;
        this->changed = true;
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
        if (vec3_equal(this->angles, angles)) return false;
        this->angles = angles;
        this->changed = true;
        return true;
    }

    PositionChangedPhysicsComponent() :
        PhysicsComponent(COMPONENT_POSITION_CHANGED),
        position(NULL_POSITION), angles(NULL_ANGLES), changed(true)
    {}
};


} // Components
