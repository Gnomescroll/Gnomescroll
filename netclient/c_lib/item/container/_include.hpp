#pragma once

//#include <item/common/constant.hpp>
//#include <item/common/enum.hpp>
//#include <item/common/struct.hpp>

//#include <item/container/properties.cpp>
#include <item/container/container.cpp>

#include <item/container/net/CtoS.cpp>
#include <item/container/net/StoC.cpp>

#include <item/container/_state.cpp>
#include <item/container/_interface.cpp>

// Client
#if DC_CLIENT
#include <item/container/client.cpp>
#include <item/container/container_ui.cpp>
#endif

// Server
#if DC_SERVER
#include <item/container/server.cpp>
#endif
