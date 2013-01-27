#pragma once

#if DC_SERVER
dont_include_this_file_in_server
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

class ColoredMinivox_list: public Simple_object_list<ColoredMinivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "ColoredMinivox"; }
    public:
        void tick();

        ColoredMinivox_list() {}
};

}   // Particle
