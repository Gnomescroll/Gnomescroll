#include "minivox_colored.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/compat_gl.h>
#include <camera/fulstrum_test.hpp>
#include <voxel/constants.hpp>
#include <physics/ray_trace/ray_trace.hpp>

namespace Particle
{

inline void ColoredMinivox::reset()
{
    this->voxel.color = MINIVOX_COLOR;
    this->ttl = MINIVOX_TTL;
    this->type = MINIVOX_TYPE;
    this->verlet.dampening = MINIVOX_DAMP;
    this->set_size(MINIVOX_SIZE);
    this->voxel.set_rotation_delta(0.0f, 0.0f);
    this->voxel.set_rotation(0.0f, 0.0f);
}

ColoredMinivox::ColoredMinivox() :
    ParticleMotion(-1, 0,0,0,0,0,0, MINIVOX_MASS*MINIVOX_SIZE)
{
    this->reset();
}

void ColoredMinivox::set_color(struct Color color)
{
    this->voxel.color = color;
}

void ColoredMinivox::set_color(unsigned char r, unsigned char g, unsigned char b)
{
    this->set_color(color_init(r,g,b));
}

void ColoredMinivox::set_size(float size)
{
    this->voxel.size = size * 0.5f; // cut size by half for centered
    this->set_mass(size * MINIVOX_MASS);
}

void ColoredMinivox::tick()
{
    struct Vec3 p = this->verlet.position; 
    this->verlet.bounce();
    if (vec3_equal(p, this->verlet.position))
        this->voxel.set_rotation_delta(0.0f, 0.0f);
    this->ttl--;
}

/* ColoredMinivox list */

void ColoredMinivox_list::tick()
{
    for (unsigned int i=0; i<num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) this->destroy(i);
    }
}

}   // Particle
