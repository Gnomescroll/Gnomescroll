#pragma once

#include <c_lib/objects/common/list/list.hpp>
#include <c_lib/objects/components/team/team.hpp>

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
