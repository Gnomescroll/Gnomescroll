/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionMomentumComponent: public PositionComponent
{
    protected:
        Vec3 momentum;
        bool momentum_changed;

    public:

    virtual bool get_momentum_changed()
    {
        return this->momentum_changed;
    }

    virtual void set_momentum_changed(bool changed)
    {
        this->momentum_changed = true;
    }

    Vec3 get_momentum()
    {
        return this->momentum;
    }

    bool set_momentum(const Vec3& momentum)
    {
        IF_ASSERT(!vec3_is_valid(momentum))
            return false;
        // don't do approximate check here, without a tiny epsilon
        // because the physics momentum deltas are very small
        if (vec3_equal(this->momentum, momentum))
            return false;
        this->momentum = momentum;
        this->momentum_changed = true;
        return true;
    }

    void load_settings_from(const Component* component)
    {
        // no settings
    }

    virtual ~PositionMomentumComponent() {}

    PositionMomentumComponent() :
        PositionComponent(COMPONENT_PositionMomentum),
        momentum(NULL_MOMENTUM)
    {}

};

} // Components
