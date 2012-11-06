#include "object_list.hpp"

#include <entity/constants.hpp>

namespace Entities
{

void EntityDataList::set_components(EntityType type, int n_components)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return;
    GS_ASSERT(n_components >= 0);
    if (n_components < 0) n_components = 0;
    if (n_components > 0)
    {
        this->component_types[type] = (ComponentType*)malloc(n_components * sizeof(ComponentType));
        this->interface_types[type] = (ComponentInterfaceType*)malloc(n_components * sizeof(ComponentInterfaceType));
    }
    this->expected_component_sizes[type] = n_components;
}

void EntityDataList::attach_component(EntityType type, ComponentType component)
{
    int index = this->component_sizes[type];
    this->component_types[type][index] = component;
    ComponentInterfaceType interface = Components::get_interface_for_component(component);
    GS_ASSERT(interface != COMPONENT_INTERFACE_NONE);
    this->interface_types[type][index] = interface;
    this->component_sizes[type] += 1;
}

inline int EntityDataList::get_component_count(EntityType type)
{
    return this->component_sizes[type];
}

inline int EntityDataList::get_component_slot(EntityType type, ComponentType component)
{
    for (int i=0; i<this->component_sizes[type]; i++)
        if (this->component_types[type][i] == component)
            return i;
    return -1;
}

inline int EntityDataList::get_component_interface_slot(EntityType type, ComponentInterfaceType interface)
{
    for (int i=0; i<this->component_sizes[type]; i++)
        if (this->interface_types[type][i] == interface)
            return i;
    return -1;
}

void EntityDataList::init()
{
    this->component_types = (ComponentType**)calloc(MAX_OBJECT_TYPES, sizeof(ComponentType*));
    this->interface_types = (ComponentInterfaceType**)calloc(MAX_OBJECT_TYPES, sizeof(ComponentInterfaceType*));
    this->expected_component_sizes = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->component_sizes = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));

    GS_ASSERT(this->component_types != NULL);
    GS_ASSERT(this->interface_types != NULL);
    GS_ASSERT(this->expected_component_sizes != NULL);
    GS_ASSERT(this->component_sizes != NULL);
}

void EntityDataList::sanity_check()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
        GS_ASSERT(this->expected_component_sizes[i] == this->component_sizes[i]);
}

EntityDataList::~EntityDataList()
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

} // Entities
