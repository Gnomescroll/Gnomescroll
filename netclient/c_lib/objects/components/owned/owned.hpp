#pragma once

#include <c_lib/objects/common/component/component.hpp>
#include <c_lib/objects/common/interface/state.hpp>
#include <c_lib/agent/constants.hpp>

// forward decl
void switch_agent_ownership(Object_types item, int owner, int new_owner);

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
