#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class StateMachineComponent: public Component
{
    public:
        EntityState state;
        stateRouter router;

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(StateMachineComponent);
        COPY(state);
        COPY(router);
    }

    virtual ~StateMachineComponent() {}

    StateMachineComponent() :
        Component(COMPONENT_StateMachine, COMPONENT_INTERFACE_StateMachine),
        state(STATE_NONE), router(NULL)
    {}
};

} // Components
