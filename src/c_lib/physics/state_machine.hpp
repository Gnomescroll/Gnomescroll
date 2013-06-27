#pragma once

#include <common/template/simple_growing_object_list.hpp>

const size_t STATE_MACHINE_NAME_MAX_LEN = 64;

typedef enum
{
    NULL_STATE_MACHINE_EVENT = -1
} StateMachineEventID;

typedef enum
{
    NULL_STATE_MACHINE_STATE = -1
} StateMachineStateID;

inline bool isValid(StateMachineEventID id)
{
    return (id != NULL_STATE_MACHINE_EVENT);
}

inline bool isValid(StateMachineStateID id)
{
    return (id != NULL_STATE_MACHINE_STATE);
}

template <typename fptr>
class StateMachineEvent
{
    public:
        StateMachineEventID id;
        char name[STATE_MACHINE_NAME_MAX_LEN];
        fptr action;
        StateMachineStateID next_state;

    StateMachineEvent() :
        id(NULL_STATE_MACHINE_EVENT), action(NULL),
        next_state(NULL_STATE_MACHINE_STATE)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};

template <typename fptr>
class StateMachineState
{
    public:
        StateMachineStateID id;
        char name[STATE_MACHINE_NAME_MAX_LEN];
        fptr action;
        SimpleGrowingObjectList<StateMachineEvent<fptr>,
                                StateMachineEventID> transitions;

    void add_event(const char* name, StateMachineStateID next_state, fptr action)
    {
        StateMachineEvent<fptr>* event = this->transitions.create();
        IF_ASSERT(event == NULL) return;
        copy_string(event->name, name, STATE_MACHINE_NAME_MAX_LEN);
        event->action = action;
        GS_ASSERT(isValid(next_state));
        event->next_state = next_state;
    }

    StateMachineEvent<fptr>* get_event(const char* name)
    {
        for (size_t i=0; i<this->transitions.count; i++)
            if (strcmp(this->transitions.objects[i].name, name) == 0)
                return &this->transitions.objects[i];
        return NULL;
    }

    StateMachineState<fptr>() :
        id(NULL_STATE_MACHINE_STATE), action(NULL),
        transitions(4, NULL_STATE_MACHINE_EVENT)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};

template <typename fptr>
class StateMachineConfiguration
{
    public:
        SimpleGrowingObjectList<StateMachineState<fptr>,
                                StateMachineStateID> states;
        StateMachineStateID start_state;

    void add_state(const char* name, fptr action)
    {   // e.g. state.add_state("wait", &tick_waiting);
        StateMachineState<fptr>* state = this->get_state(name);
        if (state == NULL)
            state = this->states.create();
        IF_ASSERT(state == NULL) return;
        if (state->name[0] == '\0')
            copy_string(state->name, name, STATE_MACHINE_NAME_MAX_LEN);
        state->action = action;
        if (this->start_state == NULL_STATE_MACHINE_STATE)
            this->start_state = state->id;
    }

    void add_transition(const char* start_state_name, const char* event_name,
                        const char* end_state_name, fptr event_action)
    {   // e.g. state.add_transition("waiting", "agent_targeted", "chase_agent", &waiting_to_chase_agent);
        StateMachineState<fptr>* start_state = this->get_state(start_state_name);
        IF_ASSERT(start_state == NULL) return;
        StateMachineState<fptr>* end_state = this->get_state(end_state_name);
        IF_ASSERT(end_state == NULL) return;
        GS_ASSERT(start_state->get_event(event_name) == NULL);
        start_state->add_event(event_name, end_state->id, event_action);
    }

    void set_start_state(const char* name)
    {   // e.g. state.set_state_state("waiting");
        StateMachineState<fptr>* state = this->get_state(name);
        IF_ASSERT(state == NULL) return;
        this->start_state = state->id;
    }

    StateMachineState<fptr>* get_state(const char* name)
    {
        for (size_t i=0; i<this->states.count; i++)
            if (strcmp(name, this->states.objects[i].name) == 0)
                return &this->states.objects[i];
        return NULL;
    }

    StateMachineConfiguration() :
        states(4, NULL_STATE_MACHINE_STATE),
        start_state(NULL_STATE_MACHINE_STATE)
    {}
};

template <typename fptr>
class StateMachine
{
    public:
        StateMachineConfiguration<fptr>* configuration;

        StateMachineStateID current_state;
        StateMachineStateID pending_state;
        void* pending_event_data;
        char pending_event[STATE_MACHINE_NAME_MAX_LEN];

    void load_configuration(StateMachineConfiguration<fptr>* configuration)
    {
        GS_ASSERT(this->configuration == NULL);
        this->configuration = configuration;
    }

    void receive_event(const char* event)
    {   // Call this when an event was incoming
        // We only store the last event received for now, because we don't
        // really have a way or reason to handle multiple per-frame events
        // We delay the event lookup, in case the state changes by the time
        // we process the event
        copy_string(this->pending_event, event, STATE_MACHINE_NAME_MAX_LEN);
    }

    void receive_event(const char* event, void* event_data)
    {
        this->receive_event(event);
        free(this->pending_event_data);
        this->pending_event_data = event_data;
    }

    fptr get_current_action()
    {   // Call this every tick to get the action function.
        // Then call the action function
        StateMachineState<fptr>* state = this->get_current_state();
        IF_ASSERT(state == NULL) return NULL;
        return state->action;
    }

    fptr get_current_event_action(void*& data)
    {   // Call this at the end of every tick to process the last event received
        // Make sure to call update() after this, so everything is properly flushed
        return this->get_pending_event_action(data);
    }

    void update()
    {   // Call this at the end of every tick to flush the event and switch
        // state, if applicable
        if (this->pending_state != NULL_STATE_MACHINE_STATE)
            this->current_state = this->pending_state;
        this->pending_state = NULL_STATE_MACHINE_STATE;
        this->pending_event[0] = '\0';
        free(this->pending_event_data);
        this->pending_event_data = NULL;
    }

    ~StateMachine()
    {
        free(this->pending_event_data);
    }

    StateMachine() :
        configuration(NULL), current_state(NULL_STATE_MACHINE_STATE),
        pending_state(NULL_STATE_MACHINE_STATE), pending_event_data(NULL)
    {
        memset(this->pending_event, 0, sizeof(this->pending_event));
    }

    private:

    fptr get_pending_event_action(void*& data)
    {
        if (this->pending_event[0] == '\0') return NULL;
        StateMachineState<fptr>* state = this->get_current_state();
        IF_ASSERT(state == NULL) return NULL;
        StateMachineEvent<fptr>* event = state->get_event(this->pending_event);
        if (event == NULL) return NULL;
        this->pending_state = event->next_state;
        data = this->pending_event_data;
        return event->action;
    }

    StateMachineState<fptr>* get_current_state()
    {
        IF_ASSERT(this->configuration == NULL) return NULL;
        return this->configuration->states.get(this->current_state);
    }

    StateMachineState<fptr>* get_state(const char* name)
    {
        IF_ASSERT(this->configuration == NULL) return NULL;
        return this->configuration->get_state(name);
    }
};
