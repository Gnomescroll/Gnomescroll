#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <toolbelt/_state.hpp>

namespace Toolbelt
{

bool toolbelt_item_alpha_action()
{
    ItemID item_id = Item::get_toolbelt_item(selected_slot);
    int item_type = Item::get_item_type(item_id);
    switch (item_type)
    {
        case 5:
            ClientState::playerAgent_state.action.hitscan_laser();
            break;
        case 7:
            ClientState::playerAgent_state.action.hitscan_pick();
            break;
        case 8: // todo -- use string names
            // player agent action throw grenade
            ClientState::playerAgent_state.action.throw_grenade();
            break;
        default:
            break;
    }
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
