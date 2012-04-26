#pragma once

/*
    Client
*/
#if DC_CLIENT

#include <c_lib/item/_interface.cpp>

//#include <c_lib/item/free_item.cpp>

#include <c_lib/item/item_container.cpp>
#include <c_lib/item/item.cpp>

//#include <c_lib/item/client/texture.cpp>
//#include <c_lib/item/client/inventory_hud.cpp>

#include <c_lib/item/net/CtoS.cpp>
#include <c_lib/item/net/StoC.cpp>

#endif

/*
    Server
*/

#if DC_SERVER

#include <c_lib/item/_interface.cpp>

//#include <c_lib/item/free_item.cpp>
#include <c_lib/item/item_container.cpp>
#include <c_lib/item/item.cpp>

#include <c_lib/item/net/CtoS.cpp>
#include <c_lib/item/net/StoC.cpp>

#endif
