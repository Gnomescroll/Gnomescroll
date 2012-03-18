#pragma once

#include <c_lib/physics/common.hpp>
#include <c_lib/physics/verlet.hpp>

using Verlet::VerletParticle;

class Particle
{
    public:
        int id;
        int ttl;
        int ttl_max;
        int type;
        float damp;

        VerletParticle* vp;

        void set_ttl(int ttl);
        void set_mass(float mass);
        Particle(int id, float x, float y, float z, float mx, float my, float mz, float mass);
        ~Particle();
};

class EventParticle: public Particle
{
    public:
        int event_ttl;

        EventParticle(int id, float x, float y, float z, float mx, float my, float mz, float mass);
};
