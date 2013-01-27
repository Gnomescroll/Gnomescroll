#pragma once

#if DC_CLIENT
dont_include_this_file_in_server
#endif

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class HealerComponent: public Component
{
    public:
        float radius;

        void heal_agents_in_range();
        void call() { this->heal_agents_in_range(); }

    HealerComponent()
    : Component(COMPONENT_HEALER, COMPONENT_INTERFACE_HEALING),
    radius(1.0f)
    {}

    virtual ~HealerComponent() {}
};

}   // Components
