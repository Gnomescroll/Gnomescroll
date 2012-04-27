#pragma once

#include <item/data/constant.hpp>
#include <item/data/enum.hpp>
#include <item/data/struct.hpp>


#include <item/properties.cpp>
#include <item/item_container.cpp>
#include <item/item.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#include <item/_state.cpp>
#include <item/_interface.cpp>


/*
    Client
*/
#if DC_CLIENT

#include <item/client.cpp>

#endif

/*
    Server
*/

#if DC_SERVER

#include <item/server.cpp>

#endif
