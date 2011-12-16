#pragma once

#include <c_lib/game/game.hpp>
#include <c_lib/game/teams.hpp>

class CTF {

    public:

        #ifdef DC_CLIENT
        bool auto_assign;
        void join_team(int team);
        void on_ready();
        #endif

        #ifdef DC_SERVER
        void on_client_connect(int client_id);
        #endif

        NoTeam none;
        CTFTeam one;
        CTFTeam two;

        void init();

        bool add_agent_to_team(int team, int agent);
        void set_team_color(int team,
            unsigned char r, unsigned char g, unsigned char b);
};

