#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <common/physics/state_machine.hpp>

typedef (*stateMachineAction)(class Entities::Entity*, class Components::StateMachineComponent*);

namespace Components
{

class StateMachineComponent: public Component
{
    public:
        // need to split StateMachine into two things:
        // StateMachineTable and StateMachine (which only holds current etc)
        StateMachineConfiguration<stateMachineAction>* configuration;
        StateMachine<stateMachineAction> state;

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(StateMachineComponent);
        this->state.load_configuration(component->state_configuration);
        this->state.current_state = other->state.current_state;
    }

    StateMachineConfiguration<stateMachineAction>* create_configuration()
    {   // only call this for the reference component
        this->configuration = new StateMachineConfiguration<stateMachineAction>;
        return this->configuration;
    }

    virtual void on_tick()
    {
        size_t n = 0;
        stateMachineAction* actions = state.get_current_actions(n);
        for (size_t i=0; i<n; i++)
            actions[i](this->entity, this);
    }

    virtual void after_tick()
    {
        size_t n = 0;
        stateMachineAction* actions = NULL;
        while ((actions = state.get_current_event_actions(n)) != NULL)
            for (size_t i=0; i<n; i++)
                actions[i](this->entity, this);
        this->state.update();
    }

    virtual ~StateMachineComponent()
    {
        delete this->configuration;
    }

    StateMachineComponent() :
        Component(COMPONENT_StateMachine, COMPONENT_INTERFACE_StateMachine),
        configuration(NULL)
    {}
};

} // Components
