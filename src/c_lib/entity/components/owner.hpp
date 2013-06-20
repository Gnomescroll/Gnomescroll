#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class OwnerComponent: public Component
{
    public:
        AgentID owner;

    AgentID get_owner()
    {
        return this->owner;
    }

    void set_owner(AgentID owner)
    {
        this->owner = owner;
    }

    #if DC_SERVER
    void revoke();
    #endif

    void load_settings_from(const Component* component)
    {
        // no settings
    }

    OwnerComponent() :
        Component(COMPONENT_Owner, COMPONENT_INTERFACE_Owner),
        owner(NULL_AGENT)
    {}
};

} // Entities
