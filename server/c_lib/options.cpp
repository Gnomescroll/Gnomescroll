#include "options.hpp"

#include <stdlib.h>

namespace Options
{

/* General */
OPT_STRING(server_name, "gnomescroll_one")

/* Network */
OPT_STRING(ip_address, "127.0.0.1")
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

}
