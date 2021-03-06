/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
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
OPT_BOOL_HEADER(compress_map)
OPT_UINT_HEADER(seed)

/* Logging */
OPT_BOOL_HEADER(logger)
OPT_BOOL_HEADER(log_chat)

/* Game */
OPT_BOOL_HEADER(pvp)
OPT_UINT_HEADER(base_move_rate);

/* Auth */
OPT_BOOL_HEADER(auth);

/* Serializer */
OPT_BOOL_HEADER(serializer);
OPT_UINT_HEADER(redis_database);

void register_options();

void validate();

}   // Options
