#include "cspray.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

const float CSPRAY_MASS = 0.1f;

void Cspray::init()
{
    this->ttl_max = CSPRAY_TTL;
    this->type = CSPRAY_TYPE;
    this->event_ttl = 1;
    this->texture_index = CSPRAY_TEXTURE_ID;
    this->scale = CSPRAY_TEXTURE_SCALE;
}

Cspray::Cspray(int id)
:
EventParticle(id, 0,0,0,0,0,0, CSPRAY_MASS),
active(0),
stopped(0)
{
    this->init();
}

Cspray::Cspray(int id, float x, float y, float z, float mx, float my, float mz)
:
EventParticle(id, x,y,z,mx,my,mz, CSPRAY_MASS),
active(0),
stopped(0)
{
    this->init();
}

void Cspray::tick()
{
    int* s;
    int collision[3];
    int tile;
    ttl++;
    s = Verlet::bounce(this->vp, collision, &tile, CSPRAY_DAMP);

    // cement effect
    if(active == 1)
    {
        ttl= ttl_max;
        if(!isActive(tile))
        {
            _set(collision[0],collision[1],collision[2], CSPRAY_CEMENT_BLOCK_TYPE);    // create block
            ttl = ttl_max;
            return;
        }
    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
        if(isActive(tile))
        {
            active=1;
            ttl *= 2;
            _set(collision[0] - s[0], collision[1] - s[1], collision[2] - s[2], CSPRAY_CEMENT_BLOCK_TYPE);    // create block
            ttl = ttl_max;
            return;
        }
    }

    /* TEST: Stop cspray falling indefinitely */
    if (this->vp->p.z < -1.0f)
    {
        this->vp->p.z = 0.0f;
        this->vp->v.x = 0.0f;
        this->vp->v.y = 0.0f;
        this->vp->v.z = 0.0f;
        ttl = ttl_max;
    }
}

/* Cspray_list */

void Cspray_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void Cspray_list::draw()
{
#ifdef DC_CLIENT
    if (num==0) return;

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture(GL_TEXTURE_2D, particle_texture_id);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_QUADS);

    for (int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw();
    
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
#endif
}
