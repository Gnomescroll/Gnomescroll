#pragma once

#include <chat/globals.hpp>

#include <chat/packets.cpp>
#include <chat/_interface.cpp>

#if DC_CLIENT
# include <chat/client.cpp>
#endif

#if DC_SERVER
# include <chat/server.cpp> 
#endif
