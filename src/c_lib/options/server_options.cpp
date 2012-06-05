#include "server_options.hpp"

#include <stdlib.h>

namespace Options
{

/* General */
OPT_STRING(server_name, "gnomescroll_one")

/* Network */
OPT_STRING(ip_address, "0.0.0.0")
OPT_INT(port, 4096)

/* Game settings */
OPT_STRING(game_mode, "ctf")
OPT_INT(number_of_teams, 2)
OPT_BOOL(team_kills, false)
OPT_INT(victory_points, 3)
OPT_STRING(team_name_one, "Blue Team")
OPT_STRING(team_name_two, "Red Team")

/* Map */
OPT_STRING(map, "natural_terrain")
OPT_UINT(seed, 8007513)
//OPT_INT(seed, 8007513)

/* Logging */
OPT_BOOL(logger, false)


void register_options()
{
    /* General */
    OPT_STRING_REGISTER(server_name)

    /* Network */
    OPT_STRING_REGISTER(ip_address)
    OPT_INT_REGISTER(port)

    /* Game settings */
    OPT_STRING_REGISTER(game_mode)
    OPT_BOOL_REGISTER(team_kills)
    OPT_INT_REGISTER(victory_points)
    OPT_STRING_REGISTER(team_name_one)
    OPT_STRING_REGISTER(team_name_two)

    /* Map */
    OPT_STRING_REGISTER(map)
    OPT_UINT_REGISTER(seed)

    /* Logging */
    OPT_BOOL_REGISTER(logger)

}

}
