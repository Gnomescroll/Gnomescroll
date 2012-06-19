#pragma once

#include <options/option_macros.hpp>

#include <common/lua/lua.hpp>

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


void register_options();

}
