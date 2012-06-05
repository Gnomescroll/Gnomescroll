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
        struct Color color;

    void explode(Vec3 position);
    void explode_random(Vec3 position);
    void explode_team(Vec3 position, int team);
    void explode_team_random(Vec3 position, int team);

    AnimationComponent()
    : Component(COMPONENT_VOXEL_ANIMATION, COMPONENT_INTERFACE_ANIMATION),
    count(10), count_max(10), size(0.5f), force(20.0f)
    {   // dont set color
    }
};

}; // Objects

#endif
