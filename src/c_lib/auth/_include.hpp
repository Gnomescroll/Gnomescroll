#pragma once

#if GS_AUTH
# include <auth/constants.hpp>
# include <auth/packets.cpp>
# include <auth/_interface.cpp>
# if DC_CLIENT
#  include <auth/client.cpp>
#  include <auth/web.cpp>
# endif
# if DC_SERVER
#  include <auth/server.cpp>
# endif
#else
# include <auth/_interface.hpp>
#endif
