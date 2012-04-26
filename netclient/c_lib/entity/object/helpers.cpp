#include "helpers.hpp"

#include <entity/component/component.hpp>
#include <entity/component/main.hpp>
#include <entity/object/object_data_list.hpp>
#include <entity/object/main.hpp>

namespace Objects
{

using Components::Component;
    
/* Object initialization helpers */

Component* add_component_to_object(Object* object, ComponentType type)
{
    Component* component = Components::get(type);
    int slot = object_data->get_component_slot(object->type, type);
    object->add_component(slot, component);
    component->object = object;
    return component;
}

void release_object_components(Object* object)
{
    for (int i=0; i<object->n_components; i++)
    {
        object->components[i]->object = NULL;
        Components::release(object->components[i]);
        object->components[i] = NULL;
    }
}

} // Objects
