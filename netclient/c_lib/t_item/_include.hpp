#pragma once

/*
    Client
*/
#if DC_CLIENT

#include <c_lib/t_item/_interface.cpp>

//#include <c_lib/t_item/free_item.cpp>

#include <c_lib/t_item/item_container.cpp>
#include <c_lib/t_item/item.cpp>

//#include <c_lib/t_item/client/texture.cpp>
//#include <c_lib/t_item/client/inventory_hud.cpp>

#include <c_lib/t_item/net/CtoS.cpp>
#include <c_lib/t_item/net/StoC.cpp>

#endif

/*
    Server
*/

#if DC_SERVER

#include <c_lib/t_item/_interface.cpp>

#include <c_lib/t_item/free_item.cpp>
#include <c_lib/t_item/item_container.cpp>
#include <c_lib/t_item/item.cpp>

#include <c_lib/t_item/net/CtoS.cpp>
#include <c_lib/t_item/net/StoC.cpp>

#endif
