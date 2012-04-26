#include "inventory.hpp"


/* Inventory Property, to use in an object */

#include <c_lib/inventory/constants.hpp>
#include <c_lib/inventory/inventory.cpp>
#include <c_lib/inventory/interface.cpp>
#include <c_lib/inventory/slot.cpp>
#include <c_lib/inventory/list.cpp>
#include <c_lib/inventory/packets.cpp>

#if DC_CLIENT
#include <c_lib/inventory/client.cpp>
#include <c_lib/inventory/handlers.cpp>
#endif

#if DC_SERVER
#include <c_lib/inventory/server.cpp>
#endif

