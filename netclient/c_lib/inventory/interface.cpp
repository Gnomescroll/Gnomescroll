#include "interface.hpp"

#include <inventory/constants.hpp>
#include <inventory/inventory.hpp>
#include <inventory/list.hpp>
#if DC_CLIENT
#include <inventory/client.hpp>
#endif
#if DC_SERVER
#include <inventory/server.hpp>
#endif

namespace Items
{

// these will hold copies of the agent_status inventory pointers
// they will be assigned on creation from server
Inventory* agent_inventory = NULL;
Inventory* agent_toolbelt = NULL;
Inventory* nanite_inventory = NULL;
Inventory* craft_bench_inventory = NULL;

/* CRUD */

InventoryList* inventory_list = NULL;

void init()
{
    assert(inventory_list == NULL);
    inventory_list = new InventoryList;
}

void teardown()
{
    if (inventory_list != NULL) delete inventory_list;
}

#if DC_SERVER
Inventory* create_inventory(ObjectType type)
{
    assert(type == OBJECT_AGENT_INVENTORY || type == OBJECT_AGENT_TOOLBELT || type == OBJECT_CRAFTING_BENCH || type == OBJECT_NANITE_INVENTORY);
    Inventory* inventory = inventory_list->create();
    inventory->type = type;
    init_inventory_dimensions(inventory);
    return inventory;
}
#endif

#if DC_CLIENT
Inventory* create_inventory(ObjectType type, int id)
{
    Inventory* inventory = inventory_list->create(id);
    inventory->type = type;
    // REMINDER: function calling this one must init inventory size
    return inventory;
}
#endif

Inventory* get_inventory(int id)
{
    return inventory_list->get(id);
}

void destroy_inventory(Inventory* inventory)
{
    destroy_inventory(inventory->id);
}

void destroy_inventory(int id)
{
    inventory_list->destroy(id);
}


InventorySlot* get_inventory_contents(int id)
{
    Inventory* inv = inventory_list->get(id);
    if (inv == NULL) return NULL;
    return inv->get_slots_array();
}

#if DC_CLIENT
#endif


}   // Items
