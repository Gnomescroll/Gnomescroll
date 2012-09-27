#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <options/option_macros.hpp>

namespace Options
{

/* General */
OPT_STRING_HEADER(server_name)

/* Network */
OPT_STRING_HEADER(ip_address)
OPT_INT_HEADER(port)

/* Map */
OPT_STRING_HEADER(map)
OPT_UINT_HEADER(seed)

/* Logging */
OPT_BOOL_HEADER(logger)
OPT_BOOL_HEADER(log_chat)

/* Game */
OPT_BOOL_HEADER(pvp)
OPT_UINT_HEADER(base_move_rate);

/* Auth */
OPT_BOOL_HEADER(auth);

void register_options();

void validate();

}   // Options
