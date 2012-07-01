#include "blood.hpp"

#include <physics/common.hpp>

namespace Particle
{

const float BLOOD_MASS = 0.1f;

inline void Blood::init()
{
    this->ttl = BLOOD_TTL;
    this->type = BLOOD_TYPE;
    this->texture_index = BLOOD_TEXTURE_ID;
    this->scale = BLOOD_TEXTURE_SCALE;
}

Blood::Blood()
:
ParticleMotion(-1, 0,0,0,0,0,0, BLOOD_MASS),
BillboardSprite()
{
    this->init();
}

void Blood::tick()
{
    this->verlet_bounce(BLOOD_DAMP);
    this->ttl--;
}

/* Blood list */

void Blood_list::tick()
{
    for (unsigned int i=0; i<num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) destroy(i);
    }
}

void Blood_list::draw()
{
    #if DC_CLIENT
    for(unsigned int i=0; i<num; i++)
        a[i].draw(a[i].get_position());
    #endif
}

}
