#pragma once

#if DC_SERVER
#error do not include this file in server
#endif

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

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(AnimationComponent)
        COPY(count)
        COPY(count_max)
        COPY(color)
        COPY(size)
        COPY(force)
    }

    AnimationComponent() :
        Component(COMPONENT_Animation, COMPONENT_INTERFACE_Animation),
        count(10), count_max(10), size(0.5f), force(20.0f)
    {   // dont set color
    }
};

} // Entities
