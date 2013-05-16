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
OPT_STRING(server_name)

/* Network */
OPT_STRING(ip_address)
OPT_INT(port, 4096)

/* Map */
OPT_STRING(map)
OPT_UINT(seed, 8007513)
OPT_BOOL(compress_map, false)

/* Logging */
OPT_BOOL(logger, true)
OPT_BOOL(log_chat, true)

/* Game */
OPT_BOOL(pvp, false)
OPT_UINT(base_move_rate, 30*60*60 /*1hr*/);

/* Auth */
OPT_BOOL(auth, true);

/* Serializer */
OPT_BOOL(serializer, true);
OPT_UINT(redis_database, 2);

void register_options()
{
    /* General */
    OPT_STRING_REGISTER(server_name, "gnomescroll_one")

    /* Network */
    OPT_STRING_REGISTER(ip_address, "0.0.0.0")
    OPT_INT_REGISTER(port)

    /* Map */
    OPT_STRING_REGISTER(map, "")
    OPT_UINT_REGISTER(seed)
    OPT_BOOL_REGISTER(compress_map)

    /* Logging */
    OPT_BOOL_REGISTER(logger)
    OPT_BOOL_REGISTER(log_chat)

    /* Game */
    OPT_BOOL_REGISTER(pvp)
    OPT_UINT_REGISTER(base_move_rate);

    /* Auth */
    OPT_BOOL_REGISTER(auth);

    /* Serialize */
    OPT_BOOL_REGISTER(serializer);
    OPT_UINT_REGISTER(redis_database);
}

void validate()
{
    GS_ASSERT(redis_database < 16);
    if (redis_database >= 16) GS_ABORT();
}

}   // Options
