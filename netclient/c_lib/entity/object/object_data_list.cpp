#include "object_list.hpp"

#include <c_lib/entity/constants.hpp>

namespace Objects
{

void ObjectDataList::set_components(ObjectType type, int n_components)
{
    this->component_types[type] = (ComponentType*)malloc(n_components * sizeof(ComponentType));
    this->interface_types[type] = (ComponentInterfaceType*)malloc(n_components * sizeof(ComponentInterfaceType));
    this->expected_component_sizes[type] = n_components;
}

void ObjectDataList::attach_component(ObjectType type, ComponentType component)
{
    int index = this->component_sizes[type];
    this->component_types[type][index] = component;
    ComponentInterfaceType interface = Components::get_interface_for_component(component);
    this->interface_types[type][index] = interface;
    this->component_sizes[type] += 1;
}

inline int ObjectDataList::get_component_count(ObjectType type)
{
    return this->component_sizes[type];
}

inline int ObjectDataList::get_component_slot(ObjectType type, ComponentType component)
{
    for (int i=0; i<this->component_sizes[type]; i++)
        if (this->component_types[type][i] == component)
            return i;
    return -1;
}

inline int ObjectDataList::get_component_interface_slot(ObjectType type, ComponentInterfaceType interface)
{
    for (int i=0; i<this->component_sizes[type]; i++)
        if (this->interface_types[type][i] == interface)
            return i;
    return -1;
}

void ObjectDataList::init()
{
    this->component_types = (ComponentType**)calloc(MAX_OBJECT_TYPES, sizeof(ComponentType*));
    this->interface_types = (ComponentInterfaceType**)calloc(MAX_OBJECT_TYPES, sizeof(ComponentInterfaceType*));
    this->expected_component_sizes = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->component_sizes = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));

    assert(this->component_types != NULL);
    assert(this->interface_types != NULL);
    assert(this->expected_component_sizes != NULL);
    assert(this->component_sizes != NULL);
}

void ObjectDataList::sanity_check()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
        assert(this->expected_component_sizes[i] == this->component_sizes[i]);
}

ObjectDataList::~ObjectDataList()
{
    if (this->component_types != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
            if (this->component_types[i] != NULL)
                free(this->component_types[i]);

        free(component_types);
    }
    
    if (this->interface_types != NULL)
    {
        for (int i=0; i<MAX_OBJECT_TYPES; i++)
            if (this->interface_types[i] != NULL)
                free(this->interface_types[i]);

        free(interface_types);
    }

    if (this->expected_component_sizes != NULL) free(this->expected_component_sizes);
    if (this->component_sizes != NULL) free(this->component_sizes);
}

} // Objects
