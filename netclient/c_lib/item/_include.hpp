#pragma once

#include <item/common/constant.hpp>
#include <item/common/enum.hpp>
#include <item/common/struct.hpp>

#include <item/properties.cpp>
#include <item/item_container.cpp>
#include <item/item.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#include <item/_state.cpp>
#include <item/_interface.cpp>

#include <item/config/config.hpp>

// Client
#if DC_CLIENT
#include <item/client.cpp>
#include <item/client_item_container.cpp>
#endif

// Server
#if DC_SERVER
#include <item/server.cpp>
#endif
