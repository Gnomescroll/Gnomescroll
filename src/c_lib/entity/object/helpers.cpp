#include "helpers.hpp"

#include <entity/component/component.hpp>
#include <entity/component/main.hpp>
#include <entity/object/object_data_list.hpp>
#include <entity/object/main.hpp>

namespace Entities
{

using Components::Component;
    
/* Entity initialization helpers */

Component* add_component_to_object(Entity* object, ComponentType type)
{
    Component* component = Components::get(type);
    GS_ASSERT(component != NULL);
    if (component == NULL) return NULL;
    int slot = entity_data->get_component_slot(object->type, type);
    object->add_component(slot, component);
    component->object = object;
    return component;
}

void release_object_components(Entity* object)
{
    for (int i=0; i<object->n_components; i++)
    {
        GS_ASSERT(object->components[i] != NULL);
        if (object->components[i] == NULL) continue;
        object->components[i]->object = NULL;
        Components::release(object->components[i]);
        object->components[i] = NULL;
    }
}

} // Entities
