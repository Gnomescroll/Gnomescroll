#pragma once

#include <c_lib/entity/constants.hpp>

namespace Components
{

class PositionMomentumPhysicsComponent: public PhysicsComponent
{
    public:
        Vec3 position;
        Vec3 momentum;
        
        Vec3 get_position() { return this->position; }
        void set_position(Vec3 position) { this->position = position; }
        Vec3 get_momentum() { return this->momentum; }
        void set_momentum(Vec3 momentum) { this->momentum = momentum; }

    PositionMomentumPhysicsComponent()
    : PhysicsComponent(COMPONENT_POSITION_MOMENTUM)
    {}
};



} // Components
