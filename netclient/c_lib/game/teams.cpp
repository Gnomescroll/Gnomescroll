#include "teams.hpp"

void Team::init(int id)
{
    this->id = id;
    n = 0;
    r = 255;
    g = 10;
    b = 10;
    
    int i;
    for(i=0; i<TEAM_MAX_PLAYERS; i++) {
        members[i] = -1;    // load default agent values
    }
}

void Team::set_name(char* name) {
    int i;
    for (i=0; i<TEAM_NAME_MAX_LENGTH || name[i] != '\0'; i++) {
        this->name[i] = name[i];
    }
    name[i] = '\0';
}

void Team::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

bool Team::has_agent(int id) {
    int i;
    for (i=0; i<TEAM_MAX_PLAYERS; i++) {
        if (members[i] == id) {
            return true;
        }
    }
    return false;
}

// returns false if team full
bool Team::add_agent(int id) {
    bool added = false;
    int i;
    for (i=0; i<TEAM_MAX_PLAYERS; i++) {
        if (members[i] == -1) {
            members[i] = id;
            added = true;
            n++;
        }
    }
    return added;
}

bool Team::remove_agent(int id) {
    bool removed = false;
    int i;
    for (i=0; i<TEAM_MAX_PLAYERS; i++) {
        if (members[i] == id) {
            members[i] = -1;
            removed = true;
            n--;
        }
    }
    return removed;
}

/* NoTeam */

void NoTeam::init(int id)
{
    n = 0;
    this->id = id;
    int i;
    for(i=0; i<GAME_MAX_PLAYERS; i++) {
        members[i] = -1;    // load default agent values
    }
    char name[] = "Viewers";
    strcpy(this->name, name);
}

// return true if agent in team
bool NoTeam::has_agent(int id) {
    int i;
    for (i=0; i<GAME_MAX_PLAYERS; i++) {
        if (members[i] == id) {
            return true;
        }
    }
    return false;
}

// returns false if team full
bool NoTeam::add_agent(int id) {
    bool added = false;
    int i;
    for (i=0; i<GAME_MAX_PLAYERS; i++) {
        if (members[i] == -1) {
            members[i] = id;
            added = true;
            n++;
        }
    }
    return added;
}

// returns true if removed
bool NoTeam::remove_agent(int id) {
    bool removed = false;
    int i;
    for (i=0; i<GAME_MAX_PLAYERS; i++) {
        if (members[i] == id) {
            members[i] = -1;
            removed = true;
            n--;
        }
    }
    return removed;
}

/* CTF Team */

void CTFTeam::init(int id)
{
    this->id = id;
    flag.team = id;
    base.team = id;
}
