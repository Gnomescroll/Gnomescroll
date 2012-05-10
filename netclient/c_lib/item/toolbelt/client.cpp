#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <item/toolbelt/_state.hpp>

namespace Toolbelt
{

bool toolbelt_item_begin_alpha_action()
{
    // in here, begin the fire tick
    // if %rate==0 do fire sound/animation

    // server will have similar looking thing,
    // which will do the actual calculations and send fire packets
    // other clients will play sound/animation as always

    // ^^ applies only for click-and-hold actions, like picking
    // single click actions like laser rifle will trigger as always

    ItemID item_id = Item::get_toolbelt_item(selected_slot);
    int item_group = Item::get_item_group(item_id);

    bool repeats = false;
    switch (item_group)
    {
        case IG_HITSCAN_WEAPON:
            ClientState::playerAgent_state.action.hitscan_laser();
            break;
        case IG_MINING_LASER:
            repeats = true;
            break;
        case IG_GRENADE_LAUNCHER:
            ClientState::playerAgent_state.action.throw_grenade();
            break;
        case IG_DEBUG:
            ClientState::set_location_pointer();
            break;
        case IG_PLACER:
            ClientState::playerAgent_state.action.set_block(item_id);
            break;
        case IG_ERROR:
            repeats = true;
            break;
        default:
            break;
    }

    if (repeats)
    {   // set tick on
        int agent_id = ClientState::playerAgent_state.agent_id;
        ASSERT_VALID_AGENT_ID(agent_id);
        agent_fire_on[agent_id] = true;
        agent_fire_tick[agent_id] = 0;
    }

    return true;
}

bool toolbelt_item_end_alpha_action()
{
    // stop advancing fire tick

    int agent_id = ClientState::playerAgent_state.agent_id;
    ASSERT_VALID_AGENT_ID(agent_id);
    agent_fire_on[agent_id] = false;
    agent_fire_tick[agent_id] = 0;

    ItemID item_id = Item::get_toolbelt_item(selected_slot);
    int item_group = Item::get_item_group(item_id);
    switch (item_group)
    {
        case IG_HITSCAN_WEAPON:
        case IG_GRENADE_LAUNCHER:
            return false;   // nothing happened. they arent click-and-hold
        case IG_MINING_LASER:
        default:
            return true;    // the default action would be click and hold (punching air)
    }
    return true;
}

bool toolbelt_item_beta_action()
{
    // beta actions are not click and hold
    // 
    return true;
}

bool toolbelt_item_reload_action()
{
    return true;
}

} // Toolbelt
