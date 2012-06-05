#pragma once

#include <entity/constants.hpp>
#include <entity/components/team.hpp>

namespace Components
{

const unsigned int TEAM_INDEX_NONE = 255;

class IndexedTeamComponent: public TeamComponent
{
    public:
        unsigned int team_index;

        unsigned int get_team_index() { return this->team_index; }
        void set_team_index(unsigned int index) { this->team_index = team_index; }

    IndexedTeamComponent()
    : TeamComponent(COMPONENT_INDEXED_TEAM),
    team_index(TEAM_INDEX_NONE)
    {}
};

} // Objects
