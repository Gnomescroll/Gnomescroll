#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class PhysicsComponent: public Component
{
    public:
        virtual Vec3 get_position() = 0;
        virtual bool set_position(Vec3 position) = 0;
        virtual Vec3 get_momentum() = 0;
        virtual bool set_momentum(Vec3 momentum) = 0;
        virtual Vec3 get_angles() = 0;
        virtual bool set_angles(Vec3 angles) = 0;


    explicit PhysicsComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_PHYSICS)
    {}

    virtual ~PhysicsComponent() {}
};

} // Components
