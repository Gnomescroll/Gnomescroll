#pragma once

#include <c_lib/game/game.hpp>
#include <c_lib/items/flag.hpp>
#include <c_lib/items/base.hpp>

const unsigned int TEAM_NAME_MAX_LENGTH = 31;
const unsigned int TEAM_MAX_PLAYERS = GAME_MAX_PLAYERS;

class Team {
    public:
        int id;
        unsigned char r,g,b;
        char name[TEAM_NAME_MAX_LENGTH + 1];

        int members[TEAM_MAX_PLAYERS];
        unsigned int n;

        bool viewers;

        unsigned int base_score;

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

        Team();
};

class NoTeam {
    public:
        int id;
        char name[7+1]; // strlen("Viewers") + '\0'

        int members[GAME_MAX_PLAYERS];
        int n;

        bool viewers;
        
        bool add_agent(int id);
        bool remove_agent(int id);
        bool has_agent(int id);

        void init(int id);

        NoTeam();
};

class CTFTeam: public Team {

    public:

        Flag* flag;
        Base* base;

        unsigned int flag_captures;

        //void set_color(unsigned char r, unsigned char g, unsigned char b);
        void init(int id);
        int score();
        void captured_flag();
        
        CTFTeam();
        ~CTFTeam();
};
