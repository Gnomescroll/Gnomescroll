#pragma once

#include <c_lib/entity/constants.hpp>

namespace Objects
{

//forward declarations
class Object;
class Component;

void add_component_to_object(Object* object, ComponentType type);
void remove_component_from_object(Object* object, Component* component);

} // Objects
