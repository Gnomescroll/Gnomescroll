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

TexturedMinivox::TexturedMinivox()
:
ParticleMotion(-1, 0,0,0,0,0,0, MINIVOX_MASS*MINIVOX_SIZE)
//,texture_pixel_width(2)
{
    this->reset();
}

void TexturedMinivox::set_size(float size)
{
    this->voxel.size = size * 0.5f;   // cut size by half for centered
    this->set_mass(size * MINIVOX_MASS);
}

#if 0
void TexturedMinivox::set_texture(int tex_id)
{
    const int pix = this->texture_pixel_width;  // NxN random texture sample within cube identified by tex_id
    const int cube_w = 32;
    const int cube_surface_w = 512;
    const int cubes_per_surface_axis = cube_surface_w / cube_w;
    const float scale = 2.0f/((float)cube_w);

    const float pixel_w = 1.0f/((float)cube_surface_w);

    this->pix_margin = pixel_w*pix;

    int itx,ity;
    float ftx,fty;

    // integer index of cube in spritesheet
    itx = tex_id % cubes_per_surface_axis;
    ity = tex_id / cubes_per_surface_axis;

    // ...scaled to texture
    ftx = itx * scale;
    fty = ity * scale;

    int mx,my;
    float fx,fy;
    // random point within cube, with margins wide enough to fit pix
    mx = (rand() % (cube_w -(pix*2))) +pix;
    my = (rand() % (cube_w -(pix*2))) +pix;

    // ...scaled to texture
    fx = ((float)mx) * pixel_w;
    fy = ((float)my) * pixel_w;

    ftx += fx;
    fty += fy;

    this->tx = ftx;
    this->ty = fty;
}
#endif

void TexturedMinivox::tick()
{
    struct Vec3 p = this->verlet.position; 
    this->verlet.bounce();
    if (vec3_equal(p, this->verlet.position))
        this->voxel.set_rotation_delta(0.0f, 0.0f);
    this->ttl--;
}

/* TexturedMinivox list */

void TexturedMinivox_list::tick()
{
    for (unsigned int i=0; i<num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) this->destroy(i);
    }
}

}   // Particle
