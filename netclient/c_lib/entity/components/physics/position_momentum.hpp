#pragma once

#include <c_lib/physics/common.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics.hpp>

namespace Components
{

class PositionMomentumPhysicsComponent: public PhysicsComponent
{
    public:
        Vec3 position;
        Vec3 momentum;
        Vec3 angles;
        
        Vec3 get_position() { return this->position; }
        void set_position(Vec3 position) { this->position = position; }

        Vec3 get_momentum() { return this->momentum; }
        void set_momentum(Vec3 momentum) { this->momentum = momentum; }

        Vec3 get_angles() { return this->angles; }
        void set_angles(Vec3 angles)
        {
            //if (vec3_equal(this->angles, angles))
                //return false;
            this->angles = angles;
            //return true;
        }


    PositionMomentumPhysicsComponent()
    : PhysicsComponent(COMPONENT_POSITION_MOMENTUM),
    position(NULL_POSITION), momentum(NULL_MOMENTUM), angles(NULL_ANGLES)
    {}
};



} // Components
