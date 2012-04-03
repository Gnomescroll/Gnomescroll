#pragma once

#include <c_lib/components/component.hpp>
#include <c_lib/entity/state.hpp>
#include <c_lib/game/game.hpp>

class TeamProperties: public ComponentProperties
{
    public:
        int team;
    TeamProperties()
    : team (NO_TEAM)
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

        void set_team(ObjectState* state, int team)
        {
            this->team_properties.team = team;
        }

    TeamComponent() {}
};
