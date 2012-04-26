#pragma once

#include <c_lib/physics/common.hpp>
#include <c_lib/physics/verlet.hpp>
#include <c_lib/physics/verlet_particle.hpp>

//#include <c_lib/objects/components/spatial/components.hpp>

namespace Particles
{

//class Particle
//{
    //public:
        //virtual void draw() = 0;
        //virtual void tick() = 0;

    //virtual ~Particle() {}
//};

class ParticleMotion
{
    public:
        Verlet::VerletComponent verlet;
        int id;
        int ttl;
        int ttl_max;
        int type;
        float damp;

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            //verlet.position = vec3_init(x,y,z);
            //verlet.velocity = vec3_init(mx,my,mz);

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

class EventParticleMotion: public ParticleMotion
{
    public:
        int event_ttl;

        EventParticleMotion(int id, float x, float y, float z, float mx, float my, float mz, float mass);
        ~EventParticleMotion(){}
};

//const int PARTICLES_MAX = 8192;
//class Particle_list: public Object_list<Particle, PARTICLES_MAX>
//{
    //private:
        //const char* name() { return "Shrapnel"; }
    //public:
        //void draw();
        //void tick();

        //Shrapnel_list() { print_list((char*)this->name(), this); }
//};


}
