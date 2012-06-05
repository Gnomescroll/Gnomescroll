#pragma once

#include <physics/common.hpp>
#include <physics/verlet.hpp>
#include <physics/verlet_particle.hpp>

namespace Particle
{

class ParticleMotion
{
    public:
        Verlet::VerletComponent verlet;
        int id;
        int ttl;
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

        ParticleMotion(int id, float x, float y, float z, float mx, float my, float mz, float mass);

        ~ParticleMotion(){}
};

}
