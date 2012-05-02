#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <toolbelt/_state.hpp>

namespace Toolbelt
{

bool toolbelt_item_begin_alpha_action()
{
    ItemID item_id = Item::get_toolbelt_item(selected_slot);
    int item_group = Item::get_item_group(item_id);
    switch (item_group)
    {
        case IG_HITSCAN_WEAPON:
            ClientState::playerAgent_state.action.hitscan_laser();
            break;
        case IG_MINING_LASER:
            ClientState::playerAgent_state.action.hitscan_pick();
            break;
        case IG_GRENADE_LAUNCHER:
            ClientState::playerAgent_state.action.throw_grenade();
            break;
        default:
            break;
    }
    return true;
}

bool toolbelt_item_end_alpha_action()
{
    return true;
}

bool toolbelt_item_beta_action()
{
    return true;
}

bool toolbelt_item_reload_action()
{
    return true;
}

} // Toolbelt
