#pragma once

#include <SDL/texture_sheet_loader.hpp>

namespace t_mob
{

class SpriteAnimation
{
    public:

        static const size_t MAX_FRAMES = 16;

        SpriteSheet sheet_id;
        int frames[MAX_FRAMES];
        int frame_count;

    SpriteAnimation()
    {

    }
};

class AnimatedSprite
{
    public:

        using class TextureSheetLoader::TextureSheetLoader;

        TextureSheetLoader texture_loader;

    void add_animation(const char* name)
    {

    }

    AnimatedSprite()
    {
    }

};

}   // t_mob
