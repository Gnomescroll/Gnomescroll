#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class WaitingComponent: public Component
{
    public:
        unsigned int tick;
        unsigned int wait_time;

    bool ready()
    {
        return this->tick >= this->wait_time;
    }

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(WaitingComponent);
        COPY(wait_time);
    }

    virtual ~WaitingComponent() {}

    WaitingComponent() :
        Component(COMPONENT_Waiting, COMPONENT_INTERFACE_Waiting),
        tick(0), wait_time(0)
    {}
};

} // Components
