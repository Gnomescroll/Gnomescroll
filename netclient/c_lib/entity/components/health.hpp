#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class HealthComponent: public Component
{
    public:

        virtual void take_damage(int dmg) = 0;
        virtual bool did_die() = 0; // should return true if life status changed to dead
                                        // and did not revive since last did_die() call
                                        // calling this should reset did_die() to false
        virtual bool is_dead() = 0; // return life state
        virtual void die() = 0;     // should set dead to true

    HealthComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_HEALTH)
    {}

    virtual ~HealthComponent() {}
};

}
