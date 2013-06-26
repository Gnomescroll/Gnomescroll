#pragma once

const size_t STATE_MACHINE_NAME_MAX_LEN = 64;

typedef enum
{
    NULL_STATE_MACHINE_EVENT = -1
} StateMachineEventID;

typedef enum
{
    NULL_STATE_MACHINE_STATE = -1
} StateMachineStateID;

typedef enum
{
    NULL_STATE_MACHINE_ACTION = -1
} StateMachineActionID;

template <typename fptr>
class StateMachineAction
{
    public:
        StateMachineActionID id;
        fptr action;

    StateMachineAction<fptr>() :
        id(NULL_STATE_MACHINE_ACTION), action(NULL)
    {}
};

template <typename fptr>
class StateMachineEvent
{
    public:
        StateMachineEventID id;
        char name[STATE_MACHINE_NAME_MAX_LEN];
        fptr action;
        StateMachineStateID next_state;

    StateMachineEvent<fptr>() :
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
        SimpleGrowingObjectList<StateMachineAction<fptr>> actions;
        SimpleGrowingObjectList<StateMachineEvent<fptr>,
                                StateMachineEventID> transitions;

    void add_event(const char* name, fptr action)
    {
        StateMachineEvent<fptr>* event = this->transitions.create();
        IF_ASSERT(event == NULL) return;
        copy_string(event->name, name, STATE_MACHINE_NAME_MAX_LEN);
        event->action = action;
    }

    StateMachineEvent<fptr>* get_event(const char* name)
    {
        for (size_t i=0; i<this->transitions.count; i++)
            if (strcmp(this->transitions.objects[i].name, name) == 0)
                return &this->transitions.objects[i];
        return NULL;
    }

    StateMachineState<fptr>() :
        id(NULL_STATE_MACHINE_STATE), actions(2, NULL),
        transitions(4, NULL_STATE_MACHINE_EVENT)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};

template <typename fptr>
class StateMachineConfiguration
{
    public:
        SimpleGrowingObjectList<StateMachineState, StateMachineStateID> states;

    void add_action(const char* name, fptr action)
    {   // e.g. state.add_state("wait", &tick_waiting);
        StateMachineState<fptr>* state = this->get_state(name);
        if (state == NULL)
            state = this->states.create();
        IF_ASSERT(state == NULL) return;
        if (state->name[0] == '\0')
            copy_string(state->name, name, STATE_MACHINE_NAME_MAX_LEN);
        StateMachineAction<fptr> state_action = state->actions.create();
        IF_ASSERT(state_action == NULL) return;
        state_action->action = action;
    }

    void add_transition(const char* start_state_name, const char* event_name,
                        const char* end_state_name, fptr event_action)
    {   // e.g. state.add_transition("waiting", "agent_targeted", "chase_agent", &waiting_to_chase_agent);
        StateMachineState<fptr>* start_state = this->get_state(start_state_name);
        IF_ASSERT(start_state == NULL) return;
        StateMachineState<fptr>* end_state = this->get_state(end_state_name);
        IF_ASSERT(end_state == NULL) return;
        GS_ASSERT(start_state->get_event(event_name) == NULL);
        start_state->add_event(event_name, event_action);
    }

    void set_start_state(const char* name)
    {   // e.g. state.set_state_state("waiting");
        StateMachineState<fptr>* state = this->get_state(name);
        IF_ASSERT(state == NULL) return;
        this->current_state = state->id;
    }

    StateMachineConfiguration<fptr>() :
        states(4, NULL_STATE_MACHINE_EVENT)
    {}
};

template <typename fptr>
class StateMachine
{
    public:
        SimpleGrowingObjectList<StateMachineState, StateMachineStateID>* states;

        StateMachineStateID current_state;
        StateMachineStateID pending_state;
        StateMachineEventID pending_event;
        char pending_event[STATE_MACHINE_NAME_MAX_LEN];

    void load_configuration(StateMachineConfiguration<fptr>* configuration)
    {
        GS_ASSERT(this->states == NULL);
        this->states = configuration;
    }

    void receive_event(const char* event)
    {   // Call this when an event was incoming
        // We only store the last event received for now, because we don't
        // really have a way or reason to handle multiple per-frame events
        // We delay the event lookup, in case the state changes by the time
        // we process the event
        copy_string(this->pending_event, event, STATE_MACHINE_NAME_MAX_LEN);
    }

    fptr* get_current_action(size_t& n_actions)
    {   // Call this every tick to get the action function.
        // Then call the action function
        n_actions = 0;
        StateMachineState<fptr>* state = this->get_current_state();
        IF_ASSERT(state == NULL) return NULL;
        n_actions = state->actions.count;
        if (!n_actions) return NULL;
        return state->actions.objects;
    }

    fptr* get_current_event_actions(size_t& n_actions)
    {   // Call this at the end of every tick to process the last event received
        // Make sure to call update() after this, so everything is properly flushed
        return this->get_pending_event_actions(n_actions);
    }

    void update()
    {   // Call this at the end of every tick to flush the event and switch
        // state, if applicable
        if (this->pending_state >= 0)
            this->current_state = this->pending_state;
        this->pending_state = 0;
        this->pending_event[0] = '\0';
    }

    ~StateMachine<fptr>()
    {
        if (this->transitions != NULL)
            for (size_t i=0; i<this->n_transitions; i++)
                free(this->transitions[i]);
        free(this->transitions);
        free(this->actions);
    }

    StateMachine<fptr>() :
        states(NULL), current_state(NULL_STATE_MACHINE_STATE),
        pending_state(NULL_STATE_MACHINE_EVENT)
    {
        memset(this->pending_event, 0, sizeof(this->pending_event));
    }

    private:

    fptr* get_pending_event_action(size_t& n_actions)
    {
        if (this->pending_event[0] == '\0') return NULL;
        StateMachineState<fptr>* state = this->get_current_state();
        IF_ASSERT(state == NULL) return NULL;
        StateMachineEvent* event = state->get_event(this->pending_event);
        IF_ASSERT(event == NULL) return NULL;
        this->pending_state = event->next_state;
        return event->action;
    }

    StateMachineState<fptr>* get_current_state()
    {
        return this->states.get(this->current_state);
    }

    StateMachineState<fptr>* get_state(const char* name)
    {
        for (size_t i=0; i<this->states.count; i++)
            if (strcmp(name, this->states.objects[i].name) == 0)
                return &this->states.objects[i];
        return NULL;
    }
};
