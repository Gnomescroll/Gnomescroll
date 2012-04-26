#pragma once

/*
    Client
*/
#if DC_CLIENT

#include <t_item/_interface.cpp>

//#include <t_item/free_item.cpp>

#include <t_item/item_container.cpp>
#include <t_item/item.cpp>

//#include <t_item/client/texture.cpp>
//#include <t_item/client/inventory_hud.cpp>

#include <t_item/net/CtoS.cpp>
#include <t_item/net/StoC.cpp>

#endif

/*
    Server
*/

#if DC_SERVER

#include <t_item/_interface.cpp>

//#include <t_item/free_item.cpp>
#include <t_item/item_container.cpp>
#include <t_item/item.cpp>

#include <t_item/net/CtoS.cpp>
#include <t_item/net/StoC.cpp>

#endif
