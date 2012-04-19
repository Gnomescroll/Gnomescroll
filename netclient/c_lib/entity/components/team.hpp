#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class TeamComponent: public Component
{
    public:
        int team;

        int get_team() { return this->team; }
        void set_team(int team) { this->team = team; }

    TeamComponent()
    : Component(COMPONENT_TEAM, COMPONENT_INTERFACE_TEAM),
    team(NO_TEAM)
    {}

    explicit TeamComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_TEAM),
    team(NO_TEAM)
    {}
};

} // Objects
