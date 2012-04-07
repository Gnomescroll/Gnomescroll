#pragma once

#include <c_lib/agent/agent_status.hpp>

#include <c_lib/lists/list.hpp>
#include <c_lib/components/owned.hpp>

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
