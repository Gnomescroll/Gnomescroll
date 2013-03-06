#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionMomentumChangedPhysicsComponent: public PhysicsComponent
{
    protected:
        Vec3 position;
        Vec3 momentum;

    public:
        Vec3 angles;
        bool changed;

    Vec3 get_position()
    {
        return this->position;
    }

    bool set_position(Vec3 position)
    {
        GS_ASSERT(vec3_is_valid(position));
        if (!vec3_is_valid(position)) return false;
        position = translate_position(position);
        if (vec3_equal(this->position, position)) return false;
        this->position = position;
        this->changed = true;
        return true;
    }

    Vec3 get_momentum()
    {
        return this->momentum;
    }

    bool set_momentum(Vec3 momentum)
    {
        GS_ASSERT(vec3_is_valid(momentum));
        if (!vec3_is_valid(momentum)) return false;
        if (vec3_equal(this->momentum, momentum)) return false;
        this->momentum = momentum;
        this->changed = true;
        return true;
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

    PositionMomentumChangedPhysicsComponent() :
        PhysicsComponent(COMPONENT_POSITION_MOMENTUM_CHANGED),
        position(NULL_POSITION), momentum(NULL_MOMENTUM), angles(NULL_ANGLES),
        changed(true)
    {}

};

} // Components
