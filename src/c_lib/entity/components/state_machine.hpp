/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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

typedef void (*stateMachineAction)(class Entities::Entity*, class Components::StateMachineComponent*, void*);

namespace Components
{

class StateMachineComponent: public Component
{
    private:
        bool owns_configuration;

    void process_current_event()
    {
        void* event_data = NULL;
        stateMachineAction action = this->state.get_current_event_action(event_data);
        if (action != NULL)
            action(this->entity, this, event_data);
        this->state.update();
    }

    public:
        // need to split StateMachine into two things:
        // StateMachineTable and StateMachine (which only holds current etc)
        StateMachineConfiguration<stateMachineAction>* configuration;
        StateMachine<stateMachineAction> state;

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(StateMachineComponent);
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
    }

    virtual void on_tick()
    {
        stateMachineAction action = state.get_current_action();
        if (action != NULL)
            action(this->entity, this, NULL);
    }

    virtual void on_after_tick()
    {
        this->process_current_event();
    }

    void receive_event(const char* name)
    {
        this->state.receive_event(name);
    }

    void receive_event(const char* name, void* event_data)
    {
        this->state.receive_event(name, event_data);
    }

    virtual ~StateMachineComponent()
    {
        if (this->owns_configuration)
            delete this->configuration;
    }

    StateMachineComponent() :
        Component(COMPONENT_StateMachine, COMPONENT_INTERFACE_StateMachine),
        owns_configuration(false), configuration(NULL)
    {}
};

} // Components
