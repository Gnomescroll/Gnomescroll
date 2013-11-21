/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <physics/common.hpp>
#include <physics/verlet.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

class VerletComponent: public PhysicsComponent
{
    public:
        Verlet::VerletComponent verlet;

    bool get_position_changed()
    {
        return true;
    }

    void set_position_changed(bool changed)
    {
    }

    bool get_angles_changed()
    {
        return true;
    }

    void set_angles_changed(bool changed)
    {
    }

    bool get_momentum_changed()
    {
        return true;
    }

    void set_momentum_changed(bool changed)
    {
    }

    Vec3 get_position()
    {
        return this->verlet.get_position();
    }

    bool set_position(const Vec3& position)
    {
        return this->verlet.set_position(position);
    }

    Vec3 get_momentum()
    {
        return this->verlet.get_momentum();
    }

    bool set_momentum(const Vec3& momentum)
    {
        return this->verlet.set_momentum(momentum);
    }

    Vec3 get_angles()
    {
        return NULL_ANGLES;
    }

    bool set_angles(const Vec3& angles)
    {
        return false;
    }

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(VerletComponent);
        this->verlet.load_settings_from(&_component->verlet);
    }

    virtual ~VerletComponent() {}

    VerletComponent() :
        PhysicsComponent(COMPONENT_Verlet)
    {}
};

} // Components
