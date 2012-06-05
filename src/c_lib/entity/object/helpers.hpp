#pragma once

#include <entity/constants.hpp>

namespace Objects
{

//forward declarations
class Object;

Component* add_component_to_object(Object* object, ComponentType type);
void release_object_components(Object* object);

} // Objects
