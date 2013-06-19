#pragma once

#if DC_CLIENT

#include <common/common.hpp>
#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class AnimationComponent: public Component
{
    public:
        int count;
        int count_max;
        float size;
        float force;
        Color color;

    void explode(Vec3 position);
    void explode_random(Vec3 position);

    AnimationComponent() :
        Component(COMPONENT_Animation, COMPONENT_INTERFACE_Animation),
        count(10), count_max(10), size(0.5f), force(20.0f)
    {   // dont set color
    }
};

}; // Entities

#endif
