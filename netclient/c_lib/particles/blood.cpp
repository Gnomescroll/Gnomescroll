#include "blood.hpp"

#include <physics/common.hpp>

namespace Particles
{

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
    this->verlet_bounce(BLOOD_DAMP);
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
    for(int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw(a[i]->get_position());
#endif
}

}
