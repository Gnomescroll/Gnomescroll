
#include "particles.hpp"

void Particle::set_ttl(int ttl)
{   // set to a negative number for no timeout
    this->ttl = ttl;
}

Particle::Particle(int id, float x, float y, float z, float ix, float iy, float iz, float mass)
:
id(id),
ttl(0)
{
    vp = new VerletParticle(x,y,z,ix,iy,iz, mass);
}

Particle::~Particle()
{
    delete vp;
}

EventParticle::EventParticle(int id, float x, float y, float z, float ix, float iy, float iz, float mass)
:
Particle(id, x,y,z,ix,iy,iz, mass)
{}
