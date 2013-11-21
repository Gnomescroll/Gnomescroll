/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "minivox_textured.hpp"

#if DC_CLIENT
#include <common/compat_gl.h>
#include <t_map/glsl/texture.hpp>
#include <camera/fulstrum_test.hpp>
#endif

#include <voxel/constants.hpp>
#include <physics/ray_trace/ray_trace.hpp>

namespace Particle
{

inline void TexturedMinivox::reset()
{
    this->ttl = MINIVOX_TTL;
    this->type = MINIVOX_TYPE;
    this->verlet.dampening = MINIVOX_DAMP;
    this->set_size(MINIVOX_SIZE);
    this->voxel.set_rotation_delta(0.0f, 0.0f);
    this->voxel.set_rotation(0.0f, 0.0f);
}

TexturedMinivox::TexturedMinivox() :
    ParticleMotion(-1, 0,0,0,0,0,0, MINIVOX_MASS*MINIVOX_SIZE)
{
    this->reset();
}

void TexturedMinivox::set_size(float size)
{
    this->voxel.size = size * 0.5f;   // cut size by half for centered
    this->set_mass(size * MINIVOX_MASS);
}

void TexturedMinivox::tick()
{
    struct Vec3 p = this->verlet.position;
    this->verlet.bounce();
    if (vec3_equal(p, this->verlet.position))
        this->voxel.set_rotation_delta(0.0f, 0.0f);
    this->ttl--;
}

/* TexturedMinivox list */

void TexturedMinivoxList::tick()
{
    for (size_t i=0; i<num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) this->destroy(i);
    }
}

}   // Particle
