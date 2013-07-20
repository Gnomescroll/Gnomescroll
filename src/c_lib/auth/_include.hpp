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
#endif
#if DC_CLIENT
# include <auth/client.cpp>
#endif
