#include "spawner.hpp"

#if DC_CLIENT

#include <c_lib/animations/animations.hpp>

namespace Components
{

void AnimationComponent::explode(Vec3 position)
{
    Animations::voxel_explode(position, this->count, this->size, this->color);
}

void AnimationComponent::explode_random(Vec3 position)
{
    Animations::voxel_explode(position, this->count, this->count_max, this->size, this->color);
}

void AnimationComponent::explode_team(Vec3 position, int team)
{
    Animations::voxel_explode_team(position, this->count, this->size, team);
}

void AnimationComponent::explode_team_random(Vec3 position, int team)
{
    Animations::voxel_explode_team(position, this->count, this->count_max, this->size, team);
}


}; // Objects

#endif
