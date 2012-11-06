#include "spawner.hpp"

#if DC_CLIENT

#include <animations/animations.hpp>

namespace Components
{

void AnimationComponent::explode(Vec3 position)
{
    Animations::voxel_explode(position, this->count, this->size, this->force, this->color);
}

void AnimationComponent::explode_random(Vec3 position)
{
    Animations::voxel_explode(position, this->count, this->count_max, this->size, this->force, this->color);
}

}; // Entities

#endif
