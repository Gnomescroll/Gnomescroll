#include "inventory_hud.hpp"

namespace t_hud
{

InventoryUI* agent_inventory = NULL;

void init_agent_inventory_ui()
{
    assert(agent_inventory == NULL);
    agent_inventory = new InventoryUI;

    agent_inventory->x = 300.0f;
    agent_inventory->y = 300.0f;

    // these will be received from network
    agent_inventory->xdim = 6;
    agent_inventory->ydim = 3;
}

    
}
