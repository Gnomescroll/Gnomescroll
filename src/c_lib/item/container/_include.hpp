#pragma once

#include <item/container/container.cpp>
#include <item/container/controller.cpp>

#include <item/container/net/CtoS.cpp>
#include <item/container/net/StoC.cpp>

#include <item/container/_state.cpp>
#include <item/container/_interface.cpp>

#include <item/container/config/types.hpp>
#include <item/container/config/_state.cpp>
#include <item/container/config/_interface.cpp>

#if DC_SERVER
# include <item/container/config/crusher_dat.cpp>
#endif

// Client
#if DC_CLIENT
# include <item/container/client.cpp>
# include <item/container/container_ui.cpp>
#endif

// Server
#if DC_SERVER
# include <item/container/server.cpp>
#endif
