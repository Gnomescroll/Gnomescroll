#pragma once

#include <c_lib/physics/vec3.hpp>

namespace Verlet
{

bool bounce(Vec3* position, Vec3* velocity, float damp);
int* bounce(Vec3* position, Vec3* velocity, float damp, int* collision, int* tile);

void bounce_box(float radius);



class VerletComponent
{
    public:

        Vec3 position;
        Vec3 velocity;
        float mass;
        
        /* Base api */
        
        Vec3 get_position()
        {
            return this->position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->position, x,y,z))
                return false;
            this->position = vec3_init(x,y,z);
            return true;
        }

        Vec3 get_momentum()
        {
            return vec3_scalar_mult(this->velocity, this->mass);
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->velocity = vec3_scalar_mult(vec3_init(mx,my,mz), 1.0f/this->mass);
        }
        
        void set_momentum(Vec3 momentum)
        {
            this->velocity = vec3_scalar_mult(momentum, 1.0f/this->mass);
        }

        float get_height()
        { return NULL_HEIGHT; }
        Vec3 get_angles() { return NULL_ANGLES; }
        bool set_angles(float theta, float phi, float rho)
        { return false; }

        /* Addition specialization */

        void set_mass(float mass)
        {
            Vec3 momentum = this->get_momentum();
            this->mass = mass;
            this->set_momentum(momentum);
        }

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            this->set_position(x,y,z);
            this->set_momentum(mx,my,mz);
        }

        bool verlet_bounce(float damp)
        {
            return Verlet::bounce(
                &this->position,
                &this->velocity,
                damp
            );
        }

    VerletComponent()
    : position(NULL_POSITION), velocity(NULL_MOMENTUM), mass(1.0f)
    {
    }
};

}

