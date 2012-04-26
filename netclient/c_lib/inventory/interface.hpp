#pragma once

/* CRUD interface for inventories */

namespace Items
{

// these will hold copies of the agent_status inventory pointers
// they will be assigned on creation from server
extern Inventory* agent_inventory;
extern Inventory* agent_toolbelt;
extern Inventory* nanite_inventory;
extern Inventory* craft_bench_inventory;

/* CRUD */

//forward decl
extern class InventoryList* inventory_list;

#if DC_SERVER
Inventory* create_inventory(ObjectType inventory_type);
#endif

#if DC_CLIENT
Inventory* create_inventory(ObjectType inventory_type, int id);
#endif

Inventory* get_inventory(int id);
void destroy_inventory(Inventory* inventory);
void destroy_inventory(int id);

#if DC_CLIENT
// call at the end of relevant input events
void process_inventory_events();
#endif

InventorySlot* get_inventory_contents(int id);


}   // Items
