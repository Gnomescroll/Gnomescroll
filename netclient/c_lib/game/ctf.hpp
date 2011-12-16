#pragma once

#include <c_lib/game/game.hpp>
#include <c_lib/game/teams.hpp>

class CTF {

    public:

        //#ifdef DC_CLIENT
        int auto_assign;
        //#endif

        NoTeam none;
        CTFTeam one;
        CTFTeam two;

        CTF();

        void add_agent_to_team(int team, int agent);
        void set_team_color(int team,
            unsigned char r, unsigned char g, unsigned char b);
};

