#include "textures.hpp"

namespace t_mob
{

class SpriteAnimationRepo* animations;

void init_animation_repo()
{
    GS_ASSERT(animations == NULL);
    animations = new class SpriteAnimationRepo;
}

void teardown_animation_repo()
{
    delete animations;
}

void SpriteAnimation::add_frame(size_t x, size_t y)
{
    IF_ASSERT(this->count >= this->MAX_FRAMES) return;
    int tex = this->group->repo->texture_loader.blit(this->sheet, x, y);
    GS_ASSERT(tex != NULL_SPRITE);
    this->frames[this->count++] = tex;
}


}   // t_mob
