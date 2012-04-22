#include "spawner.hpp"

#if DC_CLIENT

#include <c_lib/animations/animations.hpp>

namespace Components
{

void AnimationComponent::explode(Vec3 position)
{
    Animations::voxel_explode(position, this->animation_count, this->animation_size, this->animation_color);
}

void AnimationComponent::explode_random(Vec3 position)
{
    Animations::voxel_explode(position, this->animation_count, this->animation_count_max, this->animation_size, this->animation_color);
}

}; // Objects

#endif
