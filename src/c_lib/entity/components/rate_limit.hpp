#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class RateLimitComponent: public Component
{
    public:
        unsigned int tick;
        unsigned int limit;

    void call()
    {
        this->tick++;
    }

    bool allowed()
    {
        if (this->tick % this->limit == 0) return true;
        return false;
    }

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(RateLimitComponent);
        COPY(limit);
    }

    virtual ~RateLimitComponent() {}

    RateLimitComponent() :
        Component(COMPONENT_RateLimit, COMPONENT_INTERFACE_RateLimit),
        tick(0), limit(1)
    {}
};

}; // Entities
