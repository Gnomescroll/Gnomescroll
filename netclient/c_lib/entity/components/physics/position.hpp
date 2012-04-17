#pragma once

#include <c_lib/entity/constants.hpp>

namespace Components
{

class PositionPhysicsComponent: public PhysicsComponent
{
    public:
        Vec3 position;
        
        Vec3 get_position() { return this->position; }
        void set_position(Vec3 position) { this->position = position; }
        Vec3 get_momentum() { return NULL_MOMENTUM; }
        void set_momentum(Vec3 momentum) {}

    PositionPhysicsComponent()
    : PhysicsComponent(COMPONENT_POSITION)
    {}
};


} // Components
