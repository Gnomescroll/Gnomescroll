#pragma once

/* CRUD interface for inventories */

class Inventory;

namespace Items
{

//forward decl
class InventoryList;

extern InventoryList* inventory_list;

extern Inventory* current_render_inventory; // current inventory selected for rendering

// these will hold copies of the agent_status inventory pointers
// they will be assigned on creation from server
extern Inventory* agent_inventory;
extern Inventory* agent_toolbelt;
extern Inventory* nanite_inventory;
extern Inventory* craft_bench_inventory;

void set_render_inventory(Inventory* inventory);
void unset_render_inventory();

#if DC_SERVER
Inventory* create_inventory(ObjectType inventory_type);
#endif

#if DC_CLIENT
Inventory* create_inventory(ObjectType inventory_type, int id);
#endif

Inventory* get_inventory(int id);
void destroy_inventory(Inventory* inventory);
void destroy_inventory(int id);

}   // Items
