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
        Component(COMPONENT_SPRITE_MOB, COMPONENT_INTERFACE_SPRITE_MOB)
    {}
};

} // Components
