/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <chat/globals.hpp>

#include <chat/packets.cpp>
#include <chat/_interface.cpp>

#if DC_CLIENT
# include <chat/commands.cpp>
# include <chat/client.cpp>
#endif

#if DC_SERVER
# include <chat/server.cpp>
#endif
