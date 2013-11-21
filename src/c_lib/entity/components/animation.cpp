/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "animation.hpp"

#if DC_SERVER
#error do not include this file in server
#endif

#include <animations/animations.hpp>

namespace Components
{

void AnimationComponent::explode(const Vec3& position)
{
    Animations::voxel_explode(position, this->count, this->size, this->force, this->color);
}

void AnimationComponent::explode_random(const Vec3& position)
{
    Animations::voxel_explode(position, this->count, this->count_max, this->size, this->force, this->color);
}

} // Entities
