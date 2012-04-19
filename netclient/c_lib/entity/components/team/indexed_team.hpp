#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/team.hpp>

namespace Components
{

class IndexedTeamComponent: public TeamComponent
{
    public:
        int team_index;

        int get_team_index() { return this->team_index; }
        void set_team_index(int index) { this->team_index = team_index; }

    IndexedTeamComponent()
    : TeamComponent(COMPONENT_INDEXED_TEAM),
    team_index(TEAM_INDEX_NONE)
    {}
};

} // Objects
