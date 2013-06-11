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

Component* get_switch(ComponentType type);
void release_switch(Component* component);

ComponentInterfaceType get_interface_for_component(ComponentType component);

void init_interfaces();
void teardown_interfaces();
void init_components();
void teardown_components();

#ifndef COMPONENT_MAIN_INCLUDE
#ifndef _COMPONENT_MAIN_HPP
#define _COMPONENT_MAIN_HPP
# define A(NAME, LNAME, TYPE, INTERFACE, MAX) \
    typedef ComponentList<NAME, COMPONENT_##TYPE, MAX> NAME##List; \
    extern NAME##List* LNAME##_list;
COMPONENTS
#undef A
#endif
#else
#ifndef _COMPONENT_MAIN_CPP
#define _COMPONENT_MAIN_CPP

#define A(NAME, LNAME, ...) \
    NAME##List* LNAME##_list = NULL;
COMPONENTS
#undef A

static ComponentInterfaceType* component_interface_map = NULL;

Component* get_switch(ComponentType type)
{
    switch (type)
    {
        #define A(NAME, LNAME, TYPE, ...) \
            case COMPONENT_##TYPE: \
                return LNAME##_list->subscribe();
        COMPONENTS
        #undef A

        case COMPONENT_NONE:
            printf("ERROR: %s -- unknown ComponentType %d\n", __FUNCTION__, type);
            return NULL;
    }
    return NULL;
}

void release_switch(Component* component)
{
    IF_ASSERT(component == NULL) return;
    switch (component->type)
    {
        #define A(NAME, LNAME, TYPE, ...) \
            case COMPONENT_##TYPE: \
                LNAME##_list->unsubscribe((NAME*)component); \
                break;
        COMPONENTS
        #undef A

        case COMPONENT_NONE:
            printf("ERROR: %s -- unknown ComponentType %d\n", __FUNCTION__, component->type);
            break;
    }
}

ComponentInterfaceType get_interface_for_component(ComponentType component)
{
    IF_ASSERT(component_interface_map == NULL)
        return COMPONENT_INTERFACE_NONE;
    IF_ASSERT(!isValid(component) && component != COMPONENT_NONE)
        return COMPONENT_INTERFACE_NONE;
    return component_interface_map[component];
}

static void set_interface_for_component(ComponentType component, ComponentInterfaceType interface)
{
    IF_ASSERT(component_interface_map == NULL)
        return;
    IF_ASSERT(!isValid(component) && component != COMPONENT_NONE)
        return;
    component_interface_map[component] = interface;
}

void init_interfaces()
{
    GS_ASSERT(component_interface_map == NULL);
    component_interface_map = (ComponentInterfaceType*)calloc(MAX_COMPONENT_TYPES + 1, sizeof(ComponentInterfaceType));
    set_interface_for_component(COMPONENT_NONE, COMPONENT_INTERFACE_NONE);

    #define A(NAME, LNAME, TYPE, INTERFACE, MAX) \
        set_interface_for_component(COMPONENT_##TYPE, COMPONENT_INTERFACE_##INTERFACE);
    COMPONENTS
    #undef A
}

void teardown_interfaces()
{
    free(component_interface_map);
}

void init_components()
{
    #define A(NAME, LNAME, ...) \
        LNAME##_list = new NAME##List;
    COMPONENTS
    #undef A
}

void teardown_components()
{
    #define A(NAME, LNAME, ...) \
        delete LNAME##_list;
    COMPONENTS
    #undef A
}

# endif
#endif

} // Components

#undef COMPONENTS
