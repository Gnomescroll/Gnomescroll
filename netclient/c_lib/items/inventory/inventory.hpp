#pragma once

/* Inventory Property, to use in an object */

#if DC_CLIENT
#include <c_lib/items/inventory/client.hpp>
//typedef class ClientInventory Inventory;
#endif
#if DC_SERVER
#include <c_lib/items/inventory/server.hpp>
//typedef class ServerInventory Inventory;
#endif

