#include <entity/component/lists.hpp>
#include <entity/constants.hpp>

/*
 * A -- component macro for all builds
 * C -- component macro for client only
 * S -- component macro for server only
*/

#if DC_SERVER
# define S(...) A(__VA_ARGS__)
#else
# define S(...)
#endif

#if DC_CLIENT
# define C(...) A(__VA_ARGS__)
#else
# define C(...)
#endif

namespace Components
{

/* ComponentList handler switches */

Component* get(ComponentType type);
Component* create_unmanaged(ComponentType type);
void release(Component* component);

ComponentInterfaceType get_interface_for_component(ComponentType component);
ComponentList* get_component_list(ComponentType type);

void init_interfaces();
void teardown_interfaces();
void init_components();
void init_components_config();
void teardown_components();

#ifndef COMPONENT_MAIN_INCLUDE
#ifndef _COMPONENT_MAIN_HPP
#define _COMPONENT_MAIN_HPP
# define A(TYPE, INTERFACE) \
    typedef ComponentListChild<TYPE##Component, COMPONENT_##TYPE> TYPE##ComponentList;
COMPONENTS
#undef A
#endif
#else
#ifndef _COMPONENT_MAIN_CPP
#define _COMPONENT_MAIN_CPP

static ComponentInterfaceType* component_interface_map = NULL;
static ComponentList** component_lists = NULL;

Component* get(ComponentType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    IF_ASSERT(component_lists[type] == NULL) return NULL;
    return component_lists[type]->subscribe();
}

void release(Component* component)
{
    IF_ASSERT(component == NULL) return;
    ComponentType type = component->type;
    IF_ASSERT(!isValid(type)) return;
    IF_ASSERT(component_lists[type] == NULL) return;
    component_lists[type]->unsubscribe(component);
}

Component* create_unmanaged(ComponentType type)
{
    switch (type)
    {
        #define A(TYPE, ...) \
            case COMPONENT_##TYPE: \
                return new TYPE##Component;
        COMPONENTS
        #undef A

        case NULL_COMPONENT_TYPE:
        default:
            GS_ASSERT(false);
            return NULL;
    }
    return NULL;
}

ComponentInterfaceType get_interface_for_component(ComponentType component)
{
    IF_ASSERT(component_interface_map == NULL)
        return NULL_COMPONENT_INTERFACE;
    IF_ASSERT(!isValid(component) && component != NULL_COMPONENT_TYPE)
        return NULL_COMPONENT_INTERFACE;
    return component_interface_map[component];
}

static void set_interface_for_component(ComponentType component, ComponentInterfaceType interface)
{
    IF_ASSERT(component_interface_map == NULL)
        return;
    IF_ASSERT(!isValid(component) && component != NULL_COMPONENT_TYPE)
        return;
    component_interface_map[component] = interface;
}

void init_interfaces()
{
    GS_ASSERT(component_interface_map == NULL);
    component_interface_map = (ComponentInterfaceType*)calloc(MAX_COMPONENT_TYPES + 1, sizeof(ComponentInterfaceType));
    set_interface_for_component(NULL_COMPONENT_TYPE, NULL_COMPONENT_INTERFACE);

    #define A(TYPE, INTERFACE) \
        set_interface_for_component(COMPONENT_##TYPE, COMPONENT_INTERFACE_##INTERFACE);
    COMPONENTS
    #undef A
}

void teardown_interfaces()
{
    free(component_interface_map);
}

static void set_component_list_max(ComponentType type, size_t max)
{
    IF_ASSERT(!isValid(type)) return;
    IF_ASSERT(component_lists[type] == NULL) return;
    component_lists[type]->init(max);
}

void init_components_config()
{
    IF_ASSERT(component_lists == NULL) return;

    #define A(TYPE, RATE...) \
        GS_ASSERT(isValid(COMPONENT_##TYPE)); \
        if (isValid(COMPONENT_##TYPE)) { \
            GS_ASSERT(component_lists[COMPONENT_##TYPE] != NULL); \
            if (component_lists[COMPONENT_##TYPE] != NULL) \
                component_lists[COMPONENT_##TYPE]->set_autocall(RATE); \
        }
    CALL_LIST
    #undef A

    for (size_t i=0; i<MAX_COMPONENT_TYPES; i++)
    {
        if (component_lists[i] == NULL) continue;
        ComponentType type = ComponentType(i);
        size_t max = Entities::get_components_needed(type);
        if (max == 0) continue;
        set_component_list_max(type, max);
    }

}

void init_components()
{
    GS_ASSERT(component_lists == NULL);
    component_lists = (ComponentList**)calloc(MAX_COMPONENT_TYPES, sizeof(*component_lists));
    #define A(TYPE, ...) \
        GS_ASSERT(isValid(COMPONENT_##TYPE)); \
        if (isValid(COMPONENT_##TYPE)) \
            component_lists[COMPONENT_##TYPE] = new TYPE##ComponentList;
    COMPONENTS
    #undef A
}

void teardown_components()
{
    if (component_lists != NULL)
        for (size_t i=0; i<MAX_COMPONENT_TYPES; i++)
            delete component_lists[i];
    free(component_lists);
}

ComponentList* get_component_list(ComponentType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return component_lists[type];
}

void call_lists()
{
    for (size_t i=0; i<MAX_COMPONENT_TYPES; i++)
    {
        ComponentList* list = component_lists[i];
        if (list != NULL && list->autocall)
            list->call();
    }
}

# endif
#endif

} // Components

#undef COMPONENTS
#undef AUTOCALL_LIST
