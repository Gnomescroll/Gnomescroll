#pragma once

namespace Draw
{

struct TextureData
{
    float sprite_width;
    float sprite_height;
    int sprites_wide;
};

struct SpriteData
{
    int index;
    float x,y,z;
    float w,h;
};

} // Draw
