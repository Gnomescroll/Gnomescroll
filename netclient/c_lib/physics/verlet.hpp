#pragma once

#include <c_lib/physics/vec3.hpp>

namespace Verlet
{
    const float dt = 1/30.0f;
    const float a = -9.8f;

    class VerletParticle
    {
        public:
        Vec3 old_p;
        Vec3 old_v;
        Vec3 p;
        Vec3 v;

        /* reference: bottom of http://www.fisica.uniud.it/~ercolessi/md/md/node21.html */
        inline void velocity_verlet(float dt)
        {
            const Vec3 va = vec3_init(0,0,a);
            p = vec3_add(p, vec3_add(vec3_scalar_mult(v, dt), vec3_scalar_mult(va, 0.5*dt*dt)));  // r(t) + v(t)dt + (1/2)a(t)dtdt
            /* the following block is for nonconstant acceleration (spring forces) */
            //v5 = vec3_scalar_add(vv, 0.5*a*dt);
            // a(t+dt) = a [constant]
            //v = vec3_scalar_add(v5, 0.5*a*dt);
            /* end block */
            v = vec3_add(v, vec3_scalar_mult(va, 0.5*dt));
        }

        void position_verlet(float dt){}
        void leapfrog_verlet(float dt){}

        void compute()
        {
            old_p = p;
            old_v = v;
            velocity_verlet(dt);
            //printf("Pos ");vec3_print(p);
            //printf("Vel ");vec3_print(v);
            //position_verlet(dt);
            //leapfrog_verlet(dt);
        }

        // recomputes from old state, at a fraction of the timestep
        // used by bounce methods
        void recompute(float fraction)
        {
            p = old_p;
            v = old_v;
            velocity_verlet(dt*fraction);
            //position_verlet(dt*fraction);
            //leapfrog_verlet(dt*fraction);
        }

        VerletParticle(float x, float y, float z, float vx, float vy, float vz)
        {
            p.x = x;
            p.y = y;
            p.z = z;
            v.x = vx;
            v.y = vy;
            v.z = vz;
            old_p = p;
            old_v = v;
        }

    };
    
    //int* move(Vec3* v, float damp);
    bool bounce(VerletParticle* v, float damp);
}

