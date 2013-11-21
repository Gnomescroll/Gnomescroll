/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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

    virtual void on_destroy()
    {
        this->revoke();
    }
    #endif

    void load_settings_from(const Component* component)
    {
        // no settings
    }

    virtual ~OwnerComponent() {}

    OwnerComponent() :
        Component(COMPONENT_Owner, COMPONENT_INTERFACE_Owner),
        owner(NULL_AGENT)
    {}
};

} // Entities
