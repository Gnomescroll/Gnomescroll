#pragma once

#include <t_mob/sprites/_interface.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class SpriteMobComponent: public Component
{
    public:
        t_mob::SpriteMob mob;

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(SpriteMobComponent);
        this->mob.load_settings_from(&_component->mob);
    }

    virtual ~SpriteMobComponent() {}
    SpriteMobComponent() :
        Component(COMPONENT_SpriteMob, COMPONENT_INTERFACE_SpriteMob)
    {}
};

} // Components
