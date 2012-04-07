#pragma once

#include <c_lib/objects/common/component/component.hpp>


// todo -- billboard sprite draw list

class BillboardSpriteProperties: public ComponentProperties
{
    public:
        int texture_index;
        float texture_scale;
        // TODO -- add pointer to texture to bind (or index for a lookup table)

    BillboardSpriteProperties()
    : texture_index(0), texture_scale(1.0f)
    {}
};

class BillboardSpriteComponent
{
    public:
        BillboardSpriteProperties sprite_properties;

        int get_texture_index()
        {
            return this->sprite_properties.texture_index;
        }
        void set_texture_index(int texture_index)
        {
            this->sprite_properties.texture_index = texture_index;
        }

        float get_texture_scale()
        {
            return this->sprite_properties.texture_scale;
        }
        void set_texture_scale(float scale)
        {
            this->sprite_properties.texture_scale = scale;
        }
};
