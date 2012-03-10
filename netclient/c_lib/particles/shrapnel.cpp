#include "shrapnel.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#include <c_lib/camera/camera.hpp>
#endif

#include <ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

const float SHRAPNEL_MASS = 0.2f;

void Shrapnel::init()
{
    this->ttl_max = SHRAPNEL_TTL;
    this->type = SHRAPNEL_TYPE;
    this->texture_index = SHRAPNEL_TEXTURE_ID;
    this->scale = SHRAPNEL_TEXTURE_SCALE;
}

Shrapnel::Shrapnel(int id)
:
Particle(id, 0,0,0,0,0,0, SHRAPNEL_MASS)
{
    this->init();
}

Shrapnel::Shrapnel(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, SHRAPNEL_MASS)
{
    this->init();
}

void Shrapnel::tick()
{
    Verlet::bounce(this->vp, SHRAPNEL_DAMP);
    ttl++;
}

/* Shrapnel list */

void Shrapnel_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void Shrapnel_list::draw()
{
#ifdef DC_CLIENT
    if(num == 0) return;

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture(GL_TEXTURE_2D, particle_texture_id);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_QUADS);

    for(int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw();

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
#endif
}

