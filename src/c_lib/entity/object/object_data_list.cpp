#include "object_list.hpp"

#include <entity/constants.hpp>

namespace Entities
{

void EntityDataList::grow_component_list(EntityType type)
{
    IF_ASSERT(!isValid(type)) return;
    size_t size = this->component_maxes[type] * 2;
    ComponentType* ct_tmp = (ComponentType*)realloc(this->component_types[type], size * sizeof(ComponentType));
    IF_ASSERT(ct_tmp == NULL) return;
    ComponentInterfaceType* cit_tmp = (ComponentInterfaceType*)realloc(this->interface_types[type], size * sizeof(ComponentInterfaceType));
    IF_ASSERT(cit_tmp == NULL)
    {
        free(ct_tmp);
        return;
    }
    Components::Component** cmp_tmp = (Components::Component**)realloc(this->components[type], size * sizeof(**this->components));
    IF_ASSERT(cmp_tmp == NULL)
    {
        free(ct_tmp);
        free(cit_tmp);
        return;
    }

    this->components[type] = cmp_tmp;
    this->component_types[type] = ct_tmp;
    this->interface_types[type] = cit_tmp;

    for (size_t i=this->component_sizes[type]; i<size; i++)
    {
        this->component_types[type][i] = NULL_COMPONENT;
        this->interface_types[type][i] = NULL_COMPONENT_INTERFACE;
        this->components[type][i] = NULL;
    }

    this->component_maxes[type] = size;
}

void EntityDataList::begin_attaching_to(EntityType type)
{
    IF_ASSERT(!isValid(type)) return;
    this->current_type = type;
    this->component_types[type] = (ComponentType*)malloc(this->component_maxes[type] * sizeof(ComponentType));
    this->interface_types[type] = (ComponentInterfaceType*)malloc(this->component_maxes[type] * sizeof(ComponentInterfaceType));
    this->components[type] = (Components::Component**)calloc(this->component_maxes[type], sizeof(**this->components));
    for (size_t i=0; i<this->component_maxes[type]; i++)
    {
        this->component_types[type][i] = NULL_COMPONENT;
        this->interface_types[type][i] = NULL_COMPONENT_INTERFACE;
    }
}

Components::Component* EntityDataList::attach_component(EntityType type, ComponentType component)
{
    IF_ASSERT(!isValid(type)) return NULL;
    size_t index = this->component_sizes[type];
    if (index >= this->component_maxes[type])
        this->grow_component_list(type);
    IF_ASSERT(index >= this->component_maxes[type])
        return NULL;
    this->components[type][index] = Components::create_unmanaged(component);
    GS_ASSERT(this->components[type][index]->type == component);
    this->component_types[type][index] = component;
    ComponentInterfaceType interface = this->components[type][index]->interface;
    GS_ASSERT(isValid(interface));
    this->interface_types[type][index] = interface;
    this->component_sizes[type] += 1;
    return this->components[type][index];
}

inline int EntityDataList::get_component_count(EntityType type)
{
    IF_ASSERT(!isValid(type)) return 0;
    return this->component_sizes[type];
}

inline Components::Component const* const* EntityDataList::get_components(EntityType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return this->components[type];
}

inline int EntityDataList::get_component_slot(EntityType type, ComponentType component)
{
    IF_ASSERT(!isValid(type)) return -1;
    for (size_t i=0; i<this->component_sizes[type]; i++)
        if (this->component_types[type][i] == component)
            return i;
    return -1;
}

inline int EntityDataList::get_component_interface_slot(EntityType type, ComponentInterfaceType interface)
{
    IF_ASSERT(!isValid(type)) return -1;
    for (size_t i=0; i<this->component_sizes[type]; i++)
        if (this->interface_types[type][i] == interface)
            return i;
    return -1;
}

void EntityDataList::init()
{
    GS_ASSERT(this->component_types == NULL);
    GS_ASSERT(this->interface_types == NULL);
    GS_ASSERT(this->component_sizes == NULL);
    GS_ASSERT(this->component_maxes == NULL);
    GS_ASSERT(this->components == NULL);

    this->component_types = (ComponentType**)calloc(MAX_ENTITY_TYPES, sizeof(ComponentType*));
    this->interface_types = (ComponentInterfaceType**)calloc(MAX_ENTITY_TYPES, sizeof(ComponentInterfaceType*));
    this->component_sizes = (size_t*)calloc(MAX_ENTITY_TYPES, sizeof(size_t));
    this->component_maxes = (size_t*)calloc(MAX_ENTITY_TYPES, sizeof(size_t));
    this->components = (Components::Component***)calloc(MAX_ENTITY_TYPES, sizeof(*this->components));

    for (int i=0; i<MAX_ENTITY_TYPES; i++)
        this->component_maxes[i] = this->COMPONENTS_START_SIZE;
}

EntityDataList::~EntityDataList()
{
    if (this->component_types != NULL)
        for (int i=0; i<MAX_ENTITY_TYPES; i++)
            free(this->component_types[i]);
    free(component_types);

    if (this->interface_types != NULL)
        for (int i=0; i<MAX_ENTITY_TYPES; i++)
            free(this->interface_types[i]);
    free(interface_types);

    if (this->components != NULL)
        for (int i=0; i<MAX_ENTITY_TYPES; i++)
            if (this->components[i] != NULL)
            {
                for (size_t j=0; j<this->component_maxes[i]; j++)
                    if (this->components[i][j] != NULL)
                        delete this->components[i][j];
                free(this->components[i]);
            }
    free(this->components);

    free(this->component_sizes);
    free(this->component_maxes);
}

} // Entities
