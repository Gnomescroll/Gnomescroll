#pragma once

#include <c_lib/components/component.hpp>
#include <c_lib/entity/state.hpp>
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

class TeamComponent
{
    public:

        TeamProperties team_properties;

        int get_team()
        {
            return this->team_properties.team;
        }
        void set_team(int team)
        {
            this->team_properties.team = team;
        }

        unsigned int get_team_index()
        {
            return this->team_properties.team_index;
        }
        void set_team_index(unsigned int team_index)
        {
            this->team_properties.team_index = team_index;
        }

    TeamComponent() {}
};
