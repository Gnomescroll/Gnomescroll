#include "items.hpp"

/* Aggregator */

// texture/spritesheet
#if DC_CLIENT
#include <c_lib/items/texture.cpp>
#endif

// packets
#include <c_lib/items/packets.cpp>

#include <c_lib/items/flag.cpp>
#include <c_lib/items/base.cpp>
#include <c_lib/items/spawner.cpp>
#include <c_lib/items/turret.cpp>
#include <c_lib/items/pickup.cpp>

#include <c_lib/items/inventory/contents.cpp>
#include <c_lib/items/inventory/server.cpp>
#include <c_lib/items/inventory/client.cpp>
#include <c_lib/items/inventory/base.cpp>
#include <c_lib/items/inventory/packets.cpp>
#include <c_lib/items/inventory/inventory.cpp>
