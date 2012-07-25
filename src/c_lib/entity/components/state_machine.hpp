#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class StateMachineComponent: public Component
{
    public:
        EntityState state;
        void (*router) (class Object*, EntityState);
    
    virtual ~StateMachineComponent() {}
    
    StateMachineComponent()
    : Component(COMPONENT_STATE_MACHINE, COMPONENT_INTERFACE_STATE_MACHINE),
    state(STATE_NONE), router(NULL)
    {}
};

} // Components
