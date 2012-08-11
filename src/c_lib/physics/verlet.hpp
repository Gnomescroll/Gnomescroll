#pragma once

#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>

namespace Verlet
{

bool bounce(struct Vec3* position, struct Vec3* velocity, float damp);
int* bounce(struct Vec3* position, struct Vec3* velocity, float damp, int* collision, int* tile);

void bounce_box(float radius);

const float dt = 1/30.0f;
const float gravity = -9.8f;

class VerletComponent
{
    public:

        struct Vec3 position;
        struct Vec3 velocity;
        float dampening;
        float mass;
        float box_radius;
        
        /* Base api */
        
        struct Vec3 get_position()
        {
            return this->position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->position, x,y,z))
                return false;
            this->position = vec3_init(x,y,z);
            this->position = translate_position(this->position);
            return true;
        }

        bool set_position(struct Vec3 p)
        {
            GS_ASSERT(vec3_is_valid(p));
            return this->set_position(p.x, p.y, p.z);
        }

        struct Vec3 get_momentum()
        {
            return vec3_scalar_mult(this->velocity, this->mass);
        }

        bool set_momentum(float mx, float my, float mz)
        {
            return this->set_momentum(vec3_init(mx,my,mz));
        }
        
        bool set_momentum(struct Vec3 momentum)
        {
            this->velocity = vec3_scalar_mult(momentum, 1.0f/this->mass);
            return true;
        }

        float get_height()
        { return NULL_HEIGHT; }
        struct Vec3 get_angles() { return NULL_ANGLES; }
        bool set_angles(float theta, float phi, float rho)
        { return false; }

        /* Addition specialization */

        void set_mass(float mass)
        {
            struct Vec3 momentum = this->get_momentum();
            this->mass = mass;
            this->set_momentum(momentum);
        }

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            this->set_position(x,y,z);
            this->set_momentum(mx,my,mz);
        }

        void set_state(Vec3 position, Vec3 momentum)
        {
            this->set_position(position);
            this->set_momentum(momentum);
        }

        bool bounce();
        int* bounce(int* collision, int* tile);
        bool bounce_box(float gravity);
        bool bounce_box();
        bool bounce_box_no_gravity();
        bool radial(float xy, float yr);
        bool collide_no_gravity();

    VerletComponent()
    : position(NULL_POSITION), velocity(NULL_MOMENTUM),
    dampening(1.0f), mass(1.0f), box_radius(1.0f)
    {}
};

}

