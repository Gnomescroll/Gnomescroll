#pragma once

#include <auth/constants.hpp>
#include <auth/packets.cpp>
#include <auth/_interface.cpp>

#if DC_CLIENT
#include <auth/client.cpp>
#endif

#if DC_SERVER
#include <auth/server.cpp>
#endif
