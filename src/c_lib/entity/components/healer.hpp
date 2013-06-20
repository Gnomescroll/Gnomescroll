#pragma once

#if DC_CLIENT
# error Do not include this file in the server
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
    void call()
    {
        this->heal_agents_in_range();
    }

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(HealerComponent);
        COPY(radius);
    }

    HealerComponent() :
        Component(COMPONENT_Healer, COMPONENT_INTERFACE_Healer),
        radius(1.0f)
    {}

    virtual ~HealerComponent() {}
};

}   // Components
