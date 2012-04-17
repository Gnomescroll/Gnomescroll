#pragma once

#include <c_lib/entity/constants.hpp>

namespace Components
{

class PhysicsComponent: public Component
{
    public:
        virtual Vec3 get_position() = 0;
        virtual void set_position(Vec3 position) = 0;
        virtual Vec3 get_momentum() = 0;
        virtual void set_momentum(Vec3 momentum) = 0;
        virtual Vec3 get_angles() = 0;
        virtual void set_angles(Vec3 angles) = 0;


    explicit PhysicsComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_PHYSICS)
    {}

    virtual ~PhysicsComponent() {}
};

} // Components
