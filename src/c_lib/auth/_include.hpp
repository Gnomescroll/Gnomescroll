/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if GS_AUTH
# if DC_CLIENT
#  include <auth/web.cpp>
# endif
#else
# include <auth/web.hpp>
#endif

#include <auth/constants.hpp>
#include <auth/packets.cpp>
#include <auth/_interface.cpp>
#if DC_SERVER
# include <auth/server.cpp>
# include <auth/lib/memxor.c>
# include <auth/lib/sha256.c>
# include <auth/lib/hmac-sha256.c>
#endif
#if DC_CLIENT
# include <auth/client.cpp>
#endif
