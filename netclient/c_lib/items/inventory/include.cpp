#include "inventory.hpp"


/* Inventory Property, to use in an object */

#if DC_CLIENT
#include <c_lib/items/inventory/client.cpp>
#include <c_lib/items/inventory/handlers.cpp>
#endif

#if DC_SERVER
#include <c_lib/items/inventory/server.cpp>
#endif

#include <c_lib/items/inventory/list.cpp>
#include <c_lib/items/inventory/interface.cpp>
#include <c_lib/items/inventory/contents.cpp>
#include <c_lib/items/inventory/packets.cpp>
