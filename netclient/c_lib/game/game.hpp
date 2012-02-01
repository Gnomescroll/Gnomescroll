#pragma once

#include <c_lib/defines.h>

const unsigned int GAME_MAX_PLAYERS = PLAYERS_MAX;
const unsigned int N_TEAMS = 2;
extern bool team_kills;

void set_team_kills(bool team_kills);
