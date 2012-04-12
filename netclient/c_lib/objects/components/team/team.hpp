#pragma once

#include <c_lib/objects/common/component/component.hpp>
#include <c_lib/objects/common/interface/state.hpp>
#include <c_lib/game/game.hpp>

class TeamProperties: public ComponentProperties
{
    public:
        int team;
        unsigned int team_index; // id within team

    TeamProperties()
    : team (NO_TEAM), team_index(TEAM_INDEX_NONE)
    {}
};

class TeamComponent: public TeamDelegate
{
    public:

        TeamProperties properties;

        int get_team()
        {
            return this->properties.team;
        }
        void set_team(int team)
        {
            this->properties.team = team;
        }

        unsigned int get_team_index()
        {
            return this->properties.team_index;
        }
        void set_team_index(unsigned int team_index)
        {
            this->properties.team_index = team_index;
        }

    TeamComponent() {}
};
