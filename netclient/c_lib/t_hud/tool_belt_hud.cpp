#include "tool_belt_hud.hpp"

namespace t_hud
{

InventoryUI* agent_toolbelt = NULL;

void init_agent_toolbelt_ui()
{
    assert(agent_toolbelt == NULL);
    agent_toolbelt = new InventoryUI;

    agent_toolbelt->x = 500.0f;
    agent_toolbelt->y = 500.0f;

    // these will be received from network
    agent_toolbelt->xdim = 9;
    agent_toolbelt->ydim = 1;

    // TMP TODO TMP
    agent_toolbelt->inventory_id = 1;
}

}
