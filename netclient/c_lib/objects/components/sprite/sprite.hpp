#pragma once

#include <c_lib/objects/common/component/component.hpp>
#if DC_CLIENT
#include <c_lib/draw/transparent.hpp>
#endif

// todo -- billboard sprite draw list

class SpriteProperties: public ComponentProperties
{
    public:
        int sprite_index;
        float scale;
        // TODO -- add pointer to texture to bind (or sprite_index for a lookup table)

        #if DC_CLIENT
        virtual void get_sprite_data(struct Draw::SpriteData* data)
        {
            data->index = sprite_index;
            data->x = 0;
            data->y = 0;
            data->z = -0.1f;
        }
        #endif

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
