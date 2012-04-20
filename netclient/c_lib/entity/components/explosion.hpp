#pragma once

#if DC_SERVER

#include <c_lib/entity/component/component.hpp>

namespace Components
{

class ExplosionComponent: public Component
{
    public:

        float radius;
        int damage;
        float proximity_radius;
        bool harms_owner; // if true, owner of exploding object will take damage

        void explode();
        bool proximity_check(); // check if near agent; if so, die

    ExplosionComponent()
    : Component(COMPONENT_EXPLOSION, COMPONENT_INTERFACE_EXPLOSION),
    radius(1.0f), damage(0), proximity_radius(1.0f), harms_owner(false)
    {}
};

} // Components

#endif
