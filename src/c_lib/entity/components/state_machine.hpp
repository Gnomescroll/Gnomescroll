#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <physics/state_machine.hpp>
#include <entity/entities/state_machine_actions.hpp>

namespace Entities
{
class Entity;
}

namespace Components
{
class StateMachineComponent;
}

typedef void (*stateMachineAction)(class Entities::Entity*, class Components::StateMachineComponent*);

namespace Components
{

class StateMachineComponent: public Component
{
    private:
        bool owns_configuration;

    void process_current_event()
    {
        stateMachineAction action = this->state.get_current_event_action();
        if (action != NULL)
            action(this->entity, this);
        this->state.update();
    }

    public:
        // need to split StateMachine into two things:
        // StateMachineTable and StateMachine (which only holds current etc)
        StateMachineConfiguration<stateMachineAction>* configuration;
        StateMachine<stateMachineAction> state;

        // miscellaneous behaviour flags
        bool aggro;

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(StateMachineComponent);
        COPY(aggro);
        this->state.load_configuration(_component->configuration);
        this->state.current_state = _component->configuration->start_state;
    }

    virtual void use_as_default()
    {
        GS_ASSERT(this->configuration == NULL);
        this->configuration = new StateMachineConfiguration<stateMachineAction>;
        this->owns_configuration = true;
    }

    virtual void on_before_tick()
    {
        this->process_current_event();
        if (this->aggro)
            check_agent_aggro(this->entity, this);
        this->process_current_event();
    }

    virtual void on_tick()
    {
        stateMachineAction action = state.get_current_action();
        if (action != NULL)
            action(this->entity, this);
    }

    virtual void on_after_tick()
    {
        this->process_current_event();
    }

    void receive_event(const char* name)
    {
        this->state.receive_event(name);
    }

    virtual ~StateMachineComponent()
    {
        if (this->owns_configuration)
            delete this->configuration;
    }

    StateMachineComponent() :
        Component(COMPONENT_StateMachine, COMPONENT_INTERFACE_StateMachine),
        owns_configuration(false), configuration(NULL), aggro(false)
    {}
};

} // Components
