#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>
#include <c_lib/entity/component/component_list.hpp>
#include <c_lib/entity/components/include.hpp>

namespace Components
{

/*
 * Adding a component:
 *
 * Add type. Create component, either inheriting exisiting interface or creating one
 * If creating new interface, add interface type
 * typedef list from template, declare extern, intern and alloc/free
 * Add subscribe/unsubscribe to switches
 * 
 */

/* ComponentList typedefs */
 
/* Physics */
const int MAX_PHYSICS_COMPONENTS = 4096;
typedef ComponentList<PositionPhysicsComponent, MAX_PHYSICS_COMPONENTS>
    PositionPhysicsComponentList;
typedef ComponentList<PositionMomentumPhysicsComponent, MAX_PHYSICS_COMPONENTS>
    PositionMomentumPhysicsComponentList;

/* Stackables */
const int MAX_STACKABLE_COMPONENTS = 4096;
typedef ComponentList<StackableComponent, MAX_STACKABLE_COMPONENTS>
    StackableComponentList;


/* ComponentList declarations */

PositionPhysicsComponentList* position_physics_component_list = NULL;
PositionMomentumPhysicsComponentList* position_momentum_physics_component_list = NULL;

StackableComponentList* stackable_component_list = NULL;
 

/* ComponentList handler switches */

Component* get_switch(ComponentType type)
{
    switch (type)
    {
        case COMPONENT_POSITION:
            return position_physics_component_list->subscribe();
            
        case COMPONENT_POSITION_MOMENTUM:
            return position_momentum_physics_component_list->subscribe();
            
        case COMPONENT_STACKABLE:
            return stackable_component_list->subscribe();
            
        default:
            printf("ERROR: Component::get() -- unknown ComponentType %d\n", type);
            return NULL;
    }
    return NULL;
}

void release_switch(Component* component)
{
    switch (component->type)
    {
        case COMPONENT_POSITION:
            return position_physics_component_list->unsubscribe(component);
            
        case COMPONENT_POSITION_MOMENTUM:
            return position_momentum_physics_component_list->unsubscribe(component);
            
        case COMPONENT_STACKABLE:
            return stackable_component_list->unsubscribe(component);
            
        default:
            printf("ERROR: Component::get() -- unknown ComponentType %d\n", type);
            return NULL;
    }
    return NULL;
}

} // Components
