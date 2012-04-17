#pragma once

#include <c_lib/entity/constants.hpp>

namespace Components
{

class Component
{
    public:
        int id;
        ComponentType type;
        ComponentInterfaceType interface;
        Object* object;

    Component(ComponentType type, ComponentInterfaceType interface)
    : id(-1), type(type), interface(interface), object(NULL)
    {}
};


} // Components
