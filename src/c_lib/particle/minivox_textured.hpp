#pragma once

#include <physics/common.hpp>
#include <voxel/common.hpp>
#include <common/draw/textured_voxel.hpp>

namespace Particle
{

class TexturedMinivox: public ParticleMotion
{
    public:

        Draw::TexturedVoxel voxel;

    void set_size(float size);
    void tick();
    inline void reset();
    TexturedMinivox();
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class TexturedMinivox_list: public SimpleObjectList<TexturedMinivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "TexturedMinivox"; }
    public:
        void tick();

        TexturedMinivox_list() {}
};

}   // Particle
