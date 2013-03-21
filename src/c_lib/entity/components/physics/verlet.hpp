#pragma once

#include <physics/common.hpp>
#include <physics/verlet.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

class VerletPhysicsComponent: public PhysicsComponent
{
    public:
        Verlet::VerletComponent verlet;

    bool get_changed()
    {
        return true;
    }

    void set_changed(bool changed)
    {
    }

    Vec3 get_position()
    {
        return this->verlet.get_position();
    }

    bool set_position(Vec3 position)
    {
        return this->verlet.set_position(position);
    }

    Vec3 get_momentum()
    {
        return this->verlet.get_momentum();
    }

    bool set_momentum(Vec3 momentum)
    {
        return this->verlet.set_momentum(momentum);
    }

    Vec3 get_angles()
    {
        return NULL_ANGLES;
    }

    bool set_angles(Vec3 angles)
    {
        return false;
    }

    VerletPhysicsComponent() :
        PhysicsComponent(COMPONENT_VERLET)
    {}
};

} // Components
