#include "server_options.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <options/argparse.hpp>
#include <common/lua/lua.hpp>

#include <stdlib.h>

namespace Options
{

/* General */
OPT_STRING(server_name, "gnomescroll_one")

/* Network */
OPT_STRING(ip_address, "0.0.0.0")
OPT_INT(port, 4096)

/* Map */
OPT_STRING(map, "natural_terrain")
OPT_UINT(seed, 8007513)

/* Logging */
OPT_BOOL(logger, true)
OPT_BOOL(log_chat, true)

/* Game */
OPT_BOOL(pvp, false)
OPT_UINT(base_move_rate, 30*60*60 /*1hr*/);

void register_options()
{
    /* General */
    OPT_STRING_REGISTER(server_name)

    /* Network */
    OPT_STRING_REGISTER(ip_address)
    OPT_INT_REGISTER(port)

    /* Map */
    OPT_STRING_REGISTER(map)
    OPT_UINT_REGISTER(seed)

    /* Logging */
    OPT_BOOL_REGISTER(logger)
    OPT_BOOL_REGISTER(log_chat)

    /* Game */
    OPT_BOOL_REGISTER(pvp)
    OPT_UINT_REGISTER(base_move_rate);
}

}   // Options
