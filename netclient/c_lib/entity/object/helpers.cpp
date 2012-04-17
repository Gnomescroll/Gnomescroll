#include "helpers.hpp"

#include <c_lib/entity/component/component.hpp>
#include <c_lib/entity/component/main.hpp>

namespace Objects
{

using Components::Component;
    
/* Object initialization helpers */

void add_component_to_object(Object* object, ComponentType type)
{
    Component* component = Components::get(type);
    object->add_component(component);
    component->object = object;
}

void remove_component_from_object(Object* object, Component* component)
{
    component->object = NULL;
    Components::release(component);
}

} // Objects
