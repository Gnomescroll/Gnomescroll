#pragma once

#include <item/data/constant.hpp>
#include <item/data/enum.hpp>
#include <item/data/struct.hpp>

/*
    Client
*/
#if DC_CLIENT

#include <item/_state.cpp>
#include <item/_interface.cpp>

#include <item/item_container.cpp>
#include <item/item.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#include <item/client.cpp>

#endif

/*
    Server
*/

#if DC_SERVER

#include <item/_state.cpp>
#include <item/_interface.cpp>

#include <item/item_container.cpp>
#include <item/item.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#include <item/server.cpp>

#endif
