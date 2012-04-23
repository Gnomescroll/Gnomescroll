#pragma once

/* CRUD interface for inventories */

class Inventory;
#if DC_CLIENT
namespace HudInventory {
    class InventoryRender; // forward decl
}
#endif

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
class InventoryList;
extern InventoryList* inventory_list;

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
// render controller
extern Inventory* current_render_inventory; // current inventory selected for rendering
HudInventory::InventoryRender* get_render_inventory();

void set_render_inventory(Inventory* inventory);
void unset_render_inventory();
#endif

}   // Items
