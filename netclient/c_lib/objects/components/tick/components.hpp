#pragma once

#include <c_lib/objects/components/tick/properties.hpp>

class RateLimitedStateBroadcastComponent
{
    protected:
    public:
        int rate_limit_state_tick;
        int rate_limit_state_interval;   // tick # to broadcast on

    bool canSendState()
    {
        this->rate_limit_state_tick++;
        if (this->rate_limit_state_tick % this->rate_limit_state_interval == 0)
            return true;
        return false;
    }

    RateLimitedStateBroadcastComponent()
    : rate_limit_state_tick(0), rate_limit_state_interval(1)
    {}
};
