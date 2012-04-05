#pragma once

#include <c_lib/option_macros.hpp>

#include <c_lib/common/lua/lua.hpp>

namespace Options
{

/* General */
OPT_STRING_HEADER(server_name)

/* Network */
OPT_STRING_HEADER(ip_address)
OPT_INT_HEADER(port)

/* Game settings */
OPT_STRING_HEADER(game_mode)
OPT_BOOL_HEADER(team_kills)
OPT_INT_HEADER(victory_points)
OPT_STRING_HEADER(team_name_one)
OPT_STRING_HEADER(team_name_two)

/* Map */
OPT_STRING_HEADER(map)
OPT_UINT_HEADER(seed)
//OPT_INT_HEADER(seed)

/* Logging */
OPT_BOOL_HEADER(logger)


void register_options();

}
