#include "helpers.hpp"

#include <entity/component/main.hpp>
#include <entity/object/object_data_list.hpp>
#include <entity/object/main.hpp>

namespace Entities
{

/* Entity initialization helpers */

class Components::Component* add_component_to_object(Entity* object, ComponentType type)
{
    class Components::Component* component = Components::get(type);
    IF_ASSERT(component == NULL) return NULL;
    int slot = entity_data->get_component_slot(object->type, type);
    object->add_component(slot, component);
    component->object = object;
    return component;
}

void release_object_components(Entity* object)
{
    for (int i=0; i<object->n_components; i++)
    {
        IF_ASSERT(object->components[i] == NULL) continue;
        object->components[i]->object = NULL;
        Components::release(object->components[i]);
        object->components[i] = NULL;
    }
}

} // Entities
