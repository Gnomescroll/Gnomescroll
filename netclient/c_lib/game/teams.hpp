#pragma once

#include <c_lib/game/game.hpp>
#include <c_lib/items/flag.hpp>
#include <c_lib/items/base.hpp>
const int TEAM_NAME_MAX_LENGTH = 31;
const int TEAM_MAX_PLAYERS = GAME_MAX_PLAYERS;

class Team {
    public:
        int id;
        unsigned char r,g,b;
        char name[TEAM_NAME_MAX_LENGTH + 1];

        int members[TEAM_MAX_PLAYERS];
        int n;

        int score;

        void set_name(char* name);
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        
        bool add_agent(int id);
        bool remove_agent(int id);
        bool has_agent(int id);

        void init(int id);

        bool full();

        #ifdef DC_SERVER
        void update_client(int client_id);
        #endif
};

class NoTeam {
    public:
        int id;
        char name[7+1]; // strlen("Viewers") + '\0'

        int members[GAME_MAX_PLAYERS];
        int n;

        bool add_agent(int id);
        bool remove_agent(int id);
        bool has_agent(int id);

        void init(int id);
};

class CTFTeam: public Team {

    public:
        Flag flag;
        Base base;

        void init(int id);
};
