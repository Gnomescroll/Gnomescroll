#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class HealthComponent: public Component
{
    public:

        virtual bool is_dead() = 0; // return life state
        virtual void die() = 0;     // should set dead to true

    HealthComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_HEALTH)
    {}

    virtual ~HealthComponent() {}
};

}
