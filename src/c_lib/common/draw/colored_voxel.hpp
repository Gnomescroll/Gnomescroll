#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/draw/voxel.hpp>

namespace Draw
{

class ColoredVoxel: public Voxel
{
    public:
        Color color;

        void call();

        void set_color(unsigned char r, unsigned char g, unsigned char b)
        {
            this->color.r = r;
            this->color.g = g;
            this->color.b = b;
        }

    ColoredVoxel() : Voxel()
    {}

};

} // Draw
