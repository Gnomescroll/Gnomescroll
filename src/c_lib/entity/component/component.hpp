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
        class Entities::Entity* object;

    virtual void load_settings_from(const Component* component) {}

    virtual ~Component() {}

    Component(ComponentType type, ComponentInterfaceType interface) :
        id(NULL_ENTITY), type(type), interface(interface), object(NULL)
    {}
};


} // Components
