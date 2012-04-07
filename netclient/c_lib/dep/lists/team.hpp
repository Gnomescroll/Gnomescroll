#pragma once

#include <c_lib/lists/list.hpp>
#include <c_lib/components/team.hpp>

const int TEAM_LIST_MAX = 4096;
class TeamList: public BehaviourList
{
    private:
        const char* name() { return "TeamList"; }
    public:

    OwnedList()
    : BehaviourList(TEAM_LIST_MAX)
    {}
};
