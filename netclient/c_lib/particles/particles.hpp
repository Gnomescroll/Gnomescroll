#pragma once

#include <c_lib/physics/common.hpp>
#include <c_lib/physics/verlet.hpp>

#include <c_lib/objects/components/spatial/components.hpp>

namespace Particles
{

class Particle: public VerletComponent
{
    public:
        int id;
        int ttl;
        int ttl_max;
        int type;
        float damp;

        void set_ttl(int ttl);
        Particle(int id, float x, float y, float z, float mx, float my, float mz, float mass);
};

class EventParticle: public Particle
{
    public:
        int event_ttl;

        EventParticle(int id, float x, float y, float z, float mx, float my, float mz, float mass);
};

}
