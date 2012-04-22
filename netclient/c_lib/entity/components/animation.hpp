#pragma once

#if DC_CLIENT

#include <c_lib/common/common.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class AnimationComponent: public Component
{
    public:
        int animation_count;
        int animation_count_max;
        float animation_size;
        struct Color animation_color;

    void explode(Vec3 position);
    void explode_random(Vec3 position);

    AnimationComponent()
    : Component(COMPONENT_VOXEL_ANIMATION, COMPONENT_INTERFACE_ANIMATION),
    animation_count(10), animation_count_max(10), animation_size(0.5f)
    {   // dont set color
    }
};

}; // Objects

#endif
