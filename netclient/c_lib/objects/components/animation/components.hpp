#pragma once

#if DC_CLIENT

#include <c_lib/common/common.hpp>
#include <c_lib/animations/animations.hpp>

class AnimationVoxelComponent
{
    public:
        int animation_count;
        int animation_count_max;
        float animation_size;
        struct Color animation_color;

    void animation_voxel_explode(Vec3 position)
    {
        Animations::voxel_explode(position, this->animation_count, this->animation_size, this->animation_color);
    }
    
    void animation_voxel_explode_random_count(Vec3 position)
    {
        Animations::voxel_explode(position, this->animation_count, this->animation_count_max, this->animation_size, this->animation_color);
    }

    AnimationVoxelComponent()
    : animation_count(10), animation_count_max(10), animation_size(0.5f)
    {   // dont set color
    }
};

#endif
