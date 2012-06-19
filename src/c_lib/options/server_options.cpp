#include "server_options.hpp"

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

}

}
