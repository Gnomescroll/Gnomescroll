#pragma once

#include <entity/constants.hpp>

//forward decl
namespace Entities { class Entity; }

namespace Components
{

class Component
{
    public:
        ComponentID id;
        ComponentType type;
        ComponentInterfaceType interface;
        class Entities::Entity* entity;

    virtual void load_settings_from(const Component* component) = 0;
    virtual void use_as_default() {}
    virtual void call() {}

    virtual void on_ready() {}
    virtual void on_destroy() {}
    virtual void on_update() {}
    virtual void on_after_update() {}
    virtual void on_before_tick() {}
    virtual void on_after_tick() {}
    //virtual void on_tick() {}

    virtual ~Component() {}

    Component(ComponentType type, ComponentInterfaceType interface) :
        id(NULL_COMPONENT), type(type), interface(interface), entity(NULL)
    {}
};


} // Components
