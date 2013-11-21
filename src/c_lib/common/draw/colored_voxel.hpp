/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
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
