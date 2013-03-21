#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionPhysicsComponent: public PhysicsComponent
{
    protected:
        Vec3 position;
        Vec3 angles;
        bool changed;

    public:

    void set_changed(bool changed)
    {
        this->changed = changed;
    }

    bool get_changed()
    {
        return this->changed;
    }

    Vec3 get_position()
    {
        return this->position;
    }

    bool set_position(Vec3 position)
    {
        position = translate_position(position);
        if (vec3_equal(this->position, position))
            return false;
        this->position = position;
        this->changed = true;
        return this->changed;
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
        if (vec3_equal(this->angles, angles))
            return false;
        this->angles = angles;
        this->changed = true;
        return this->changed;
    }

    PositionPhysicsComponent() :
        PhysicsComponent(COMPONENT_POSITION)
    {
        this->_init();
    }

    explicit PositionPhysicsComponent(ComponentType type) :
        PhysicsComponent(type)
    {
        this->_init();
    }

    private:
    void _init()
    {
        this->position = NULL_POSITION;
        this->angles = NULL_ANGLES;
        this->changed = true;
    }
};

} // Components
