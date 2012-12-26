#pragma once

namespace Badges
{

const size_t BADGE_NAME_MAX_LENGTH = 63;
const size_t MAX_BADGES = 64;

class Badge
{
    public:
        char name[BADGE_NAME_MAX_LENGTH+1];
        int sprite_id;

    Badge() : sprite_id(NULL_SPRITE)
    {
        memset(name, 0, sizeof(this->name));
    }
};

extern size_t n_badges;
extern class Badge badges[MAX_BADGES];

void register_badges();

}   // Badges
