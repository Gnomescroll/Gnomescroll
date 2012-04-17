#include "shrapnel.hpp"

#include <physics/common.hpp>

#include <c_lib/physics/verlet_particle.hpp>

namespace Particles
{

using VerletParticle::VerletParticle;

const float SHRAPNEL_MASS = 0.2f;

void Shrapnel::init()
{
    verlet.dampening = SHRAPNEL_DAMP;

    verlet.position = vec3_init(x,y,z);
    verlet.velocity = vec3_init(mx,my,mz);

    this->ttl = SHRAPNEL_TTL;
    //this->type = SHRAPNEL_TYPE;
    //this->texture_index = SHRAPNEL_TEXTURE_ID;
    //this->scale = SHRAPNEL_TEXTURE_SCALE;
}

void Shrapnel::tick()
{
    this->verlet.bounce();
    ttl--;
}

void Shrapnel::draw()
{
    this->verlet.bounce();
    ttl--;
}

}

/* Shrapnel list */

namespace Particles
{

void Shrapnel_list::tick()
{
    for(int i=0; i<this->num; i++)
    {
        a[i]->tick();
        if (a[i]->ttl <= 0)
            destroy(a[i]->id);
    }
}

void Shrapnel_list::prep()
{
#if DC_CLIENT
    for(int i=0; i<this->num; i++)
    {

    }
#endif
}


void Shrapnel_list::draw()
{
#if DC_CLIENT
    if(num == 0) return;

    assert(particle_texture != 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, particle_texture);
    glBegin(GL_QUADS);

    for(int i=0; i<this->num; i++)
    {
        a[i]->draw();
    }

    glEnd();
    glDisable(GL_BLEND);
#endif
}

}
