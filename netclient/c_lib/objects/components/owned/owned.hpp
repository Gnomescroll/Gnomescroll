#pragma once

#include <c_lib/objects/common/component/component.hpp>
#include <c_lib/objects/common/interface/state.hpp>
#include <c_lib/agent/constants.hpp>

// forward decl
void switch_agent_ownership(int id, Object_types item, int subtype, int owner, int new_owner);

class OwnedProperties: public ComponentProperties
{
    public:
        int owner;
    OwnedProperties()
    : owner(NO_AGENT)
    {}
};

class OwnedComponent: public OwnedDelegate
{
    public:
    
    OwnedProperties properties;

    int get_owner()
    {
        return this->properties.owner;
    }

    void set_owner(int id, Object_types type, int subtype, int owner)
    {
        switch_agent_ownership(id, type, subtype, this->properties.owner, owner);
        this->properties.owner = owner;
    }

    OwnedComponent() {}
};
