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

    Component(ComponentType type, ComponentInterfaceType interface)
    : id(-1), type(type), interface(interface), object(NULL)
    {}
};


} // Components
