#pragma once

#include <c_lib/physics/vec3.hpp>

namespace Verlet
{

const float dt = 1/30.0f;
const float a = -9.8f;

class VerletParticle
{
    private:
        /* reference: bottom of http://www.fisica.uniud.it/~ercolessi/md/md/node21.html */
        inline void velocity_verlet(float dt);
        //inline void position_verlet(float dt);
        //inline void leapfrog_verlet(float dt);

    public:
        Vec3 old_p;
        Vec3 old_v;
        Vec3 p; // position
        Vec3 v; // velocity
        float mass;

        void compute();
        void recompute(float fraction);
        void set_position(float x, float y, float z);
        void set_momentum(float mx, float my, float mz);
        void set_state(float x, float y, float z, float mx, float my, float mz);
        void set_mass(float mass);
        Vec3 get_momentum();
        
        void init_state(float x, float y, float z, float mx, float my, float mz);
        void init_state(float x, float y, float z, float mx, float my, float mz, float mass);


        VerletParticle() {}
        VerletParticle(float x, float y, float z, float mx, float my, float mz);
        VerletParticle(float x, float y, float z, float mx, float my, float mz, float mass);
};

//int* move(Vec3* v, float damp);
bool bounce(VerletParticle* v, float damp);
int* bounce(VerletParticle* v, int* collision, int* tile, float damp);

}

