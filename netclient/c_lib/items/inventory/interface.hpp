#pragma once

/* CRUD interface for inventories */

class Inventory;

namespace Items
{

//forward decl
class InventoryList;

extern InventoryList* inventory_list;

extern Inventory* render_inventory; // current inventory selected for rendering

void begin_render(Inventory* inventory);
void end_render();

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
