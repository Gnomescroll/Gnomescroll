#include "main.hpp"

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

/* ComponentList declarations */

PositionPhysicsComponentList* position_physics_component_list = NULL;
PositionMomentumPhysicsComponentList* position_momentum_physics_component_list = NULL;
VerletPhysicsComponentList* verlet_physics_component_list = NULL;

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
            
        case COMPONENT_VERLET:
            return verlet_physics_component_list->subscribe();
            
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
            position_physics_component_list->unsubscribe((PositionPhysicsComponent*)component);
            break;
            
        case COMPONENT_POSITION_MOMENTUM:
            position_momentum_physics_component_list->unsubscribe((PositionMomentumPhysicsComponent*)component);
            break;

        case COMPONENT_VERLET:
            verlet_physics_component_list->unsubscribe((VerletPhysicsComponent*)component);
            break;
            
        case COMPONENT_STACKABLE:
            stackable_component_list->unsubscribe((StackableComponent*)component);
            break;
            
        default:
            printf("ERROR: Component::get() -- unknown ComponentType %d\n", component->type);
            break;
    }
}

void init()
{
    position_physics_component_list = new PositionPhysicsComponentList;
    position_momentum_physics_component_list = new PositionMomentumPhysicsComponentList;
    verlet_physics_component_list = new VerletPhysicsComponentList;

    stackable_component_list = new StackableComponentList;
}

void teardown()
{
    if (position_physics_component_list != NULL) delete position_physics_component_list;
    if (position_momentum_physics_component_list != NULL) delete position_momentum_physics_component_list;
    if (verlet_physics_component_list != NULL) delete  verlet_physics_component_list;
    
    if (stackable_component_list != NULL) delete stackable_component_list;
}

} // Components
