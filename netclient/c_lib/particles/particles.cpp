#include "particles.hpp"

namespace Particles
{

void Particle::set_ttl(int ttl)
{   // set to a negative number for no timeout
    this->ttl = ttl;
}

Particle::Particle(int id, float x, float y, float z, float mx, float my, float mz, float mass)
:
id(id),
ttl(0)
{
    this->verlet_properties.mass = mass;    // dont use set_mass() because dont need to recompute velocity
    this->set_position(x,y,z);
    this->set_momentum(mx,my,mz);
}

EventParticle::EventParticle(int id, float x, float y, float z, float mx, float my, float mz, float mass)
:
Particle(id, x,y,z,mx,my,mz, mass)
{}

}
