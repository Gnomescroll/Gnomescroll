#pragma once

#include <c_lib/agent/agent_status.hpp>

#include <c_lib/lists/list.hpp>

class OwnedProperties: public ListProperties
{
    public:
        int owner;
    OwnedProperties()
    : owner(NO_AGENT)
    {}
};

class OwnedComponent
{
    public:
    
    OwnedProperties owned_properties;

    int get_owner()
    {
        return this->owned_properties.owner;
    }

    void set_owner(ObjectState* state, int owner)
    {
        switch_agent_ownership(state->type, this->owned_properties.owner, owner);
        this->owned_properties.owner = owner;
    }

    OwnedComponent() {}
};

const int OWNED_LIST_MAX = 4096;
class OwnedList: public BehaviourList
{
    private:
        const char* name() { return "OwnedList"; }
    public:
        void transfer_ownership(int owner, int new_owner);

    OwnedList()
    : BehaviourList(OWNED_LIST_MAX)
    {}
};
