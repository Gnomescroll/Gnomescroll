#pragma once

/* Handlers for inventory events that require cutting across modules */

#if DC_CLIENT

// forward decl
class Inventory;

namespace items
{

// call this on netowrk inventory create
void received_inventory_handler(Inventory* inventory);

// call at end of input event
void inventory_input_event();

} // Items
#endif
