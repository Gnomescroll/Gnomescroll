#include "shrapnel.hpp"

#include <physics/common.hpp>

namespace Particles
{

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
Particle(id, 0,0,0,0,0,0, SHRAPNEL_MASS),
BillboardSprite()
{
    this->init();
}

Shrapnel::Shrapnel(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, SHRAPNEL_MASS),
BillboardSprite()
{
    this->init();
}

void Shrapnel::tick()
{
    this->verlet_bounce(SHRAPNEL_DAMP);
    ttl++;
}

}

/* Shrapnel list */

namespace Particles
{

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
    for(int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw(a[i]->get_position());
#endif
}

}
