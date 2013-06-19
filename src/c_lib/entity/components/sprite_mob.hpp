#pragma once

#include <t_mob/sprites/_interface.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class SpriteMobComponent: public Component
{
    public:
        t_mob::SpriteMob mob;

    virtual ~SpriteMobComponent() {}
    SpriteMobComponent() :
        Component(COMPONENT_SpriteMob, COMPONENT_INTERFACE_SpriteMob)
    {}
};

} // Components
