#pragma once

#include <physics/common.hpp>
#include <physics/verlet.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>

//#include <physics/quadrant.hpp>

namespace Components
{

class VerletPhysicsComponent: public PhysicsComponent
{
    public:
        Verlet::VerletComponent verlet;

        Vec3 get_position() { return this->verlet.get_position(); }
        bool set_position(Vec3 position)
        {
            return this->verlet.set_position(position);
        }

        Vec3 get_momentum()
        {
            return this->verlet.get_momentum();
        }
        bool set_momentum(Vec3 momentum)
        {
            return this->verlet.set_momentum(momentum);
        }

        Vec3 get_angles() { return NULL_ANGLES; }
        bool set_angles(Vec3 angles) { return false; }

        //void set_mass(float mass)
        //{
            //Vec3 momentum = this->get_momentum();
            //this->mass = mass;
            //this->set_momentum(momentum);
        //}

        //void set_state(Vec3 position, Vec3 momentum)
        //{
            //this->set_position(position);
            //this->set_momentum(momentum);
        //}

        //bool bounce()
        //{
            //return Verlet::bounce(
                //&this->position,
                //&this->velocity,
                //this->damp
            //);
        //}

        //bool bounce_box()
        //{
            //return Verlet::bounce_box(
                //&this->position,
                //&this->velocity,
                //this->damp,
                //this->box_radius
            //);
        //}

    VerletPhysicsComponent()
    : PhysicsComponent(COMPONENT_VERLET)
    //position(NULL_POSITION), velocity(NULL_MOMENTUM),
    //mass(1.0f), damp(1.0f), box_radius(0.1f)
    {}
};

} // Components
