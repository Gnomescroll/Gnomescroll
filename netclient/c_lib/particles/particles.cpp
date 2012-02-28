
#include "particles.hpp"

void Particle::set_ttl(int ttl)
{   // set to a negative number for no timeout
    this->ttl = ttl;
}

Particle::Particle(int id, float x, float y, float z, float vx, float vy, float vz)
:
id(id),
ttl(0)
{
    vp = new VerletParticle(x,y,z,vx,vy,vz);
}

Particle::~Particle()
{
    delete vp;
}

EventParticle::EventParticle(int id, float x, float y, float z, float vx, float vy, float vz)
:
Particle(id, x,y,z,vx,vy,vz)
{}
