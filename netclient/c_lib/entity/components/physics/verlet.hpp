#pragma once

#include <physics/common.hpp>
#include <physics/verlet.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

class VerletPhysicsComponent: public PhysicsComponent
{
    public:
        Vec3 position;
        Vec3 velocity;
        float mass;
        float damp;
        float box_radius;
        
        Vec3 get_position() { return this->position; }
        bool set_position(Vec3 position)
        {
            this->position = position;
            return true;
        }

        Vec3 get_momentum()
        {
            return vec3_scalar_mult(this->velocity, this->mass);
        }
        bool set_momentum(Vec3 momentum)
        {
            this->velocity = vec3_scalar_mult(momentum, 1.0f/this->mass);
            return true;
        }

        Vec3 get_angles() { return NULL_ANGLES; }
        bool set_angles(Vec3 angles) { return false; }

        /* Addition specialization */

        void set_mass(float mass)
        {
            Vec3 momentum = this->get_momentum();
            this->mass = mass;
            this->set_momentum(momentum);
        }

        void set_state(Vec3 position, Vec3 momentum)
        {
            this->set_position(position);
            this->set_momentum(momentum);
        }

        bool bounce()
        {
            return this->bounce_box();
            return Verlet::bounce(
                &this->position,
                &this->velocity,
                this->damp
            );
        }

        bool bounce_box()
        {
            return Verlet::bounce_box(
                &this->position,
                &this->velocity,
                this->damp,
                this->box_radius
            );
        }

    VerletPhysicsComponent()
    : PhysicsComponent(COMPONENT_VERLET),
    position(NULL_POSITION), velocity(NULL_MOMENTUM),
    mass(1.0f), damp(1.0f), box_radius(0.1f)
    {}
};

} // Components
