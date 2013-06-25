#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class PhysicsComponent: public Component
{
    public:

    virtual Vec3 get_position() = 0;
    virtual bool set_position(const Vec3& position) = 0;
    virtual Vec3 get_momentum() = 0;
    virtual bool set_momentum(const Vec3& momentum) = 0;
    virtual Vec3 get_angles() = 0;
    virtual bool set_angles(const Vec3& angles) = 0;

    virtual void set_changed(bool changed)
    {
        this->set_position_changed(changed);
        this->set_angles_changed(changed);
        this->set_momentum_changed(changed);
    }

    virtual bool get_changed()
    {
        return (this->get_position_changed() ||
                this->get_momentum_changed() ||
                this->get_angles_changed());
    }

    virtual void set_position_changed(bool changed) = 0;
    virtual bool get_position_changed() = 0;
    virtual void set_momentum_changed(bool changed) = 0;
    virtual bool get_momentum_changed() = 0;
    virtual void set_angles_changed(bool changed) = 0;
    virtual bool get_angles_changed() = 0;

    virtual void on_after_update()
    {
        this->set_changed(false);
    }

    explicit PhysicsComponent(ComponentType type) :
        Component(type, COMPONENT_INTERFACE_Physics)
    {}

    virtual ~PhysicsComponent() {}
};

} // Components
