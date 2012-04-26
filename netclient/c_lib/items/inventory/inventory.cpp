#include "inventory.hpp"

namespace Items
{

void init_inventory_dimensions(class BaseInventory* inventory)
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
            assert(false);
            break;
    }
}

} // Items
