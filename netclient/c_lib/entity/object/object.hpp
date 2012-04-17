#pragma once

#include <c_lib/entity/constants.hpp>

//forward declaration
namespace Components
{
    class Component;
}

namespace Objects
{

using Components::Component;

class Object
{
    public:
        int n_components;
        Component** components;

        int id;
        ObjectType type;

        void add_component(Component* component);
        Component* get_component(ComponentType type);
        Component* get_component_interface(ComponentInterfaceType interface);

        void init(int n_components);

    ~Object();

    explicit Object(int id)
    : n_components(0), components(NULL), id(id), type(OBJECT_NONE)
    {}
};


} // Objects
