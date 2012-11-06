#pragma once

#include <entity/constants.hpp>

namespace Entities
{

//forward declarations
class Entity;

Component* add_component_to_object(Entity* object, ComponentType type);
void release_object_components(Entity* object);

} // Entities
