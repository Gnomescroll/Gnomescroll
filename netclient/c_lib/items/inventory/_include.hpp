#include "inventory.hpp"


/* Inventory Property, to use in an object */

#include <c_lib/items/inventory/constants.hpp>
#include <c_lib/items/inventory/inventory.cpp>
#include <c_lib/items/inventory/interface.cpp>
#include <c_lib/items/inventory/slot.cpp>
#include <c_lib/items/inventory/list.cpp>
#include <c_lib/items/inventory/packets.cpp>

#if DC_CLIENT
#include <c_lib/items/inventory/client.cpp>
#include <c_lib/items/inventory/handlers.cpp>
#endif

#if DC_SERVER
#include <c_lib/items/inventory/server.cpp>
#endif

