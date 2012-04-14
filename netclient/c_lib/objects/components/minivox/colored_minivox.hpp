#pragma once

#if DC_CLIENT

#include <c_lib/objects/components/minivox/minivox.hpp>

// todo -- minivox color draw list

class ColoredMinivoxProperties: public MinivoxProperties
{
    public:
        Color color;

    ColoredMinivoxProperties()
    : MinivoxProperties()
    {}
};

class ColoredMinivoxComponent: public MinivoxComponent<ColoredMinivoxProperties>
{
    public:
        Color get_color()
        {
            return this->minivox_properties.color;
        }

        void set_color(Color color)
        {
            this->minivox_properties.color = color;
        }

        void set_color(unsigned char r, unsigned char g, unsigned char b)
        {
            Color color;
            color.r = r;
            color.g = g;
            color.b = b;
            this->minivox_properties.color = color;
        }

    ColoredMinivoxComponent()
    : MinivoxComponent<ColoredMinivoxProperties>()
    {
    }
};

#endif
