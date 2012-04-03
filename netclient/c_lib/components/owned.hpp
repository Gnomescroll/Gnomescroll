#pragma once

#include <c_lib/components/component.hpp>
#include <c_lib/entity/state.hpp>
#include <c_lib/agent/agent_status.hpp>
#include <c_lib/agent/constants.hpp>

class OwnedProperties: public ComponentProperties
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

    void set_owner(Object_types type, int owner)
    {
        switch_agent_ownership(type, this->owned_properties.owner, owner);
        this->owned_properties.owner = owner;
    }

    OwnedComponent() {}
};
