#include "blood.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

const float BLOOD_MASS = 0.1f;

void Blood::init()
{
    this->ttl_max = BLOOD_TTL;
    this->type = BLOOD_TYPE;
    this->texture_index = BLOOD_TEXTURE_ID;
    this->scale = BLOOD_TEXTURE_SCALE;
}

Blood::Blood(int id)
:
Particle(id, 0,0,0,0,0,0, BLOOD_MASS),
BillboardSprite()
{
    this->init();
}

Blood::Blood(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, BLOOD_MASS),
BillboardSprite()
{
    this->init();
}

void Blood::tick()
{
    Verlet::bounce(this->vp, BLOOD_DAMP);
    this->ttl++;
}

/* Blood list */

void Blood_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void Blood_list::draw()
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
            a[i]->draw(a[i]->vp->p);

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
#endif
}

