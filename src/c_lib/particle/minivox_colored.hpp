/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <common/color.hpp>
#include <physics/common.hpp>
#include <voxel/common.hpp>
#include <common/draw/textured_voxel.hpp>
#include <common/template/object_list.hpp>

namespace Particle
{

const int MINIVOX_MAX = 4096*2;
const int MINIVOX_TTL = 100;
const float MINIVOX_DAMP = 0.5f;
const int MINIVOX_TYPE = 6;

/* voxel properties */
const float MINIVOX_MASS = 2.0f;
const float MINIVOX_SIZE = 0.05f;
const Color MINIVOX_COLOR = Color(136, 27, 224);

class ColoredMinivox: public ParticleMotion
{
    public:

        Draw::ColoredVoxel voxel;

    void set_color(Color color);
    void set_color(unsigned char r, unsigned char g, unsigned char b);

    void set_size(float size);

    void tick();

    inline void reset();
    ColoredMinivox();
};

class ColoredMinivoxList: public SimpleObjectList<ColoredMinivox, MINIVOX_MAX>
{
    public:
    void tick();

    private:
    const char* name()
    {
        return "ColoredMinivox";
    }
};

}   // Particle
