#pragma once

#if DC_CLIENT

#include <c_lib/objects/components/minivox/minivox.hpp>

// todo -- minivox color draw list

class TexturedMinivoxProperties: public MinivoxProperties
{
    public:
        int texture_index; // for cube
        int pixel_width;
        
        float pixel_margin;   // not a config, cached value
        float tx,ty;
        
    TexturedMinivoxProperties()
    : MinivoxProperties(),
    texture_index(0), pixel_width(1),
    pixel_margin(0), tx(0), ty(0)
    {}
};

class TexturedMinivoxComponent: public MinivoxComponent<TexturedMinivoxProperties>
{
    public:

    void set_texture()
    {
        const int pix = this->minivox_properties.pixel_width;  // NxN random texture sample within cube identified by texture_index
        const int cube_w = 32;
        const int cube_surface_w = 512;
        const int cubes_per_surface_axis = cube_surface_w / cube_w;
        const float scale = 2.0f/((float)cube_w);

        const float pixel_w = 1.0f/((float)cube_surface_w);

        this->minivox_properties.pixel_margin = pixel_w*pix;

        int itx,ity;
        float ftx,fty;

        // integer index of cube in spritesheet
        itx = this->minivox_properties.texture_index % cubes_per_surface_axis;
        ity = this->minivox_properties.texture_index / cubes_per_surface_axis;

        // ...scaled to texture
        ftx = itx * scale;
        fty = ity * scale;

        int mx,my;
        float fx,fy;
        // random point within cube, with margins wide enough to fit pix
        mx = (rand() % (cube_w -(pix*2))) + pix;
        my = (rand() % (cube_w -(pix*2))) + pix;

        // ...scaled to texture
        fx = ((float)mx) * pixel_w;
        fy = ((float)my) * pixel_w;

        ftx += fx;
        fty += fy;

        this->minivox_properties.tx = ftx;
        this->minivox_properties.ty = fty;
    }

    TexturedMinivoxComponent()
    : MinivoxComponent<TexturedMinivoxProperties>()
    {
    }
};

#endif
