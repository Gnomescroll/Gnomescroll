#include "interface.hpp"

#include <c_lib/items/inventory/constants.hpp>
#include <c_lib/items/inventory/inventory.hpp>
#include <c_lib/items/inventory/list.hpp>

/* CRUD interface for inventories */

namespace Items
{

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

static void init_inventory_dimensions(Inventory* inventory)
{
    switch (inventory->type)
    {
        case OBJECT_AGENT_INVENTORY:
            inventory->init(AGENT_INVENTORY_X, AGENT_INVENTORY_Y);
            break;
        case OBJECT_AGENT_TOOLBELT:
            inventory->init(AGENT_TOOLBELT_X, AGENT_TOOLBELT_Y);
            break;
        case OBJECT_CRAFTING_BENCH:
            inventory->init(CRAFTING_BENCH_X, CRAFTING_BENCH_Y);
            break;
        case OBJECT_NANITE_INVENTORY:
            inventory->init(NANITE_INVENTORY_X, NANITE_INVENTORY_Y);
            break;
            
        default:
            printf("WARNING: Items::create_inventory() -- unknown inventory type %d\n", inventory->type);
            break;
    }
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
    printf("creating inventory %d,%d\n", type, id);
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
    
}   // Items
