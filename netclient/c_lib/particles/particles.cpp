#include "particles.hpp"

namespace Particles
{

void ParticleMotion::set_ttl(int ttl)
{   // set to a negative number for no timeout
    this->ttl = ttl;
}

ParticleMotion::ParticleMotion(int id, float x, float y, float z, float mx, float my, float mz, float mass)
:
id(id),
ttl(0)
{
    this->verlet.mass = mass;    // dont use set_mass() because dont need to recompute velocity
    this->set_position(x,y,z);
    this->set_momentum(mx,my,mz);
}

}
