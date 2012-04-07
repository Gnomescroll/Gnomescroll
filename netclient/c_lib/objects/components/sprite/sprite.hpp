#pragma once

#include <c_lib/objects/common/component/component.hpp>


// todo -- billboard sprite draw list

class SpriteProperties: public ComponentProperties
{
    public:
        int sprite_index;
        float scale;
        // TODO -- add pointer to texture to bind (or sprite_index for a lookup table)

    SpriteProperties()
    : sprite_index(0), scale(1.0f)
    {}
};

class SpriteComponent
{
    public:
        SpriteProperties sprite_properties;

        int get_sprite_index()
        {
            return this->sprite_properties.sprite_index;
        }
        void set_sprite_index(int sprite_index)
        {
            this->sprite_properties.sprite_index = sprite_index;
        }

        float get_scale()
        {
            return this->sprite_properties.scale;
        }
        void set_scale(float scale)
        {
            this->sprite_properties.scale = scale;
        }
};
