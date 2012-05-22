#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <item/toolbelt/_state.hpp>

namespace Toolbelt
{

bool toolbelt_item_begin_alpha_action()
{
    int agent_id = ClientState::playerAgent_state.agent_id;
    if (agent_id < 0 || agent_id >= AGENT_MAX) return false;
    ASSERT_VALID_AGENT_ID(agent_id);
    // set tick on
    agent_fire_on[agent_id] = true;
    agent_fire_tick[agent_id] = 0;

    return true;
}

bool toolbelt_item_end_alpha_action()
{
    // stop advancing fire tick

    int agent_id = ClientState::playerAgent_state.agent_id;
    ASSERT_VALID_AGENT_ID(agent_id);
    agent_fire_on[agent_id] = false;
    agent_fire_tick[agent_id] = 0;

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    int item_group;
    if (item_id == NULL_ITEM) item_group = IG_NONE;
    else item_group = Item::get_item_group(item_id);
    switch (item_group)
    {
        case IG_MINING_LASER:
        case IG_SHOVEL:
        case IG_NONE:
            return true;
            
        case IG_HITSCAN_WEAPON:
        case IG_GRENADE_LAUNCHER:
            return false;   // nothing happened. they arent click-and-hold
        default:
            return false;    // the default action is click once
    }
    return true;
}

bool toolbelt_item_beta_action()
{
    // open any inventories in range

    using ClientState::playerAgent_state;
    int container_id = playerAgent_state.facing_container();
    if (container_id == NULL_CONTAINER) return false;

    ItemContainer::open_container(container_id);
    return true;
}

bool toolbelt_item_reload_action()
{
    return true;
}

} // Toolbelt
