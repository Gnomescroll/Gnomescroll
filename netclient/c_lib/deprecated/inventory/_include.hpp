#include "inventory.hpp"


/* Inventory Property, to use in an object */

#include <inventory/constants.hpp>
#include <inventory/inventory.cpp>
#include <inventory/interface.cpp>
#include <inventory/slot.cpp>
#include <inventory/list.cpp>
#include <inventory/packets.cpp>

#if DC_CLIENT
#include <inventory/client.cpp>
#include <inventory/handlers.cpp>
#endif

#if DC_SERVER
#include <inventory/server.cpp>
#endif

