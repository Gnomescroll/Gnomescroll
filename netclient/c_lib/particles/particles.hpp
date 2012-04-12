#pragma once

#include <c_lib/physics/common.hpp>
#include <c_lib/physics/verlet.hpp>

#include <c_lib/objects/components/spatial/components.hpp>

namespace Particles
{

class Particle
{
    public:
        VerletComponent verlet;
        int id;
        int ttl;
        int ttl_max;
        int type;
        float damp;

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            this->verlet.set_state(x,y,z,mx,my,mz);
        }

        void set_position(float x, float y, float z)
        {
            this->verlet.set_position(x,y,z);
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->verlet.set_momentum(mx,my,mz);
        }

        void set_mass(float mass)
        {
            this->verlet.set_mass(mass);
        }

        Vec3 get_position()
        {
            return this->verlet.get_position();
        }

        Vec3 get_momentum()
        {
            return this->verlet.get_momentum();
        }

        bool verlet_bounce(float damp)
        {
            return this->verlet.verlet_bounce(damp);
        }

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
