#pragma once

#include <entity/constants.hpp>

//forward decl
namespace Entities { class Entity; }

namespace Components
{

class Component
{
    public:
        int id;
        ComponentType type;
        ComponentInterfaceType interface;
        class Entities::Entity* entity;

    virtual void load_settings_from(const Component* component) = 0;
    virtual void use_as_default() {}

    virtual ~Component() {}

    Component(ComponentType type, ComponentInterfaceType interface) :
        id(NULL_ENTITY), type(type), interface(interface), entity(NULL)
    {}
};


} // Components
