/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "particles.hpp"

namespace Particle
{

void ParticleMotionCore::set_ttl(int ttl)
{   // set to a negative number for no timeout
    this->ttl = ttl;
}

ParticleMotionCore::ParticleMotionCore(float x, float y, float z, float mx, float my, float mz, float mass)
: ttl(0)
{
    this->verlet.mass = mass;    // dont use set_mass() because dont need to recompute velocity
    this->set_position(x,y,z);
    this->set_momentum(mx,my,mz);
}

}   // Particle
