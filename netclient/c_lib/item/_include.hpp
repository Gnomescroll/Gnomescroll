#pragma once

/*
    Client
*/
#if DC_CLIENT

#include <item/_interface.cpp>

//#include <item/free_item.cpp>

#include <item/item_container.cpp>
#include <item/item.cpp>

//#include <item/client/texture.cpp>
//#include <item/client/inventory_hud.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#endif

/*
    Server
*/

#if DC_SERVER

#include <item/_interface.cpp>

//#include <item/free_item.cpp>
#include <item/item_container.cpp>
#include <item/item.cpp>

#include <item/net/CtoS.cpp>
#include <item/net/StoC.cpp>

#endif
