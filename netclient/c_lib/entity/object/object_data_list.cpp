#include "object_list.hpp"

#include <c_lib/entity/constants.hpp>

namespace Objects
{

void ObjectDataList::set_components(ObjectType type, int n_components)
{
    this->component_types[type] = (ComponentType*)malloc(n_components * sizeof(ComponentType));
    this->expected_component_sizes[type] = n_components;
}

void ObjectDataList::attach_component(ObjectType type, ComponentType component)
{
    int index = this->component_sizes[type];
    this->component_types[type][index] = component;
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
    printf("ObjectDataList::get_component_slot() -- Did not find component %d for object type %d\n", component, type);
    assert(false);
    return -1;
}

void ObjectDataList::init()
{
    this->component_types = (ComponentType**)calloc(MAX_OBJECT_TYPES, sizeof(ComponentType*));
    this->expected_component_sizes = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
    this->component_sizes = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
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

    if (this->expected_component_sizes != NULL) free(this->expected_component_sizes);
    if (this->component_sizes != NULL) free(this->component_sizes);
}

} // Objects
