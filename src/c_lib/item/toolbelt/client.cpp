#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <item/toolbelt/_state.hpp>
#include <chat/interface.hpp>

namespace Toolbelt
{

void turn_fire_on(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    agent_fire_tick[agent_id] = 0;

    if (agent_fire_on[agent_id]) return;

    agent_fire_on[agent_id] = true;

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemGroup item_group;
    if (item_id == NULL_ITEM) item_group = IG_NONE;
    else item_group = Item::get_item_group(item_id);
    if (ClientState::playerAgent_state.agent_id == agent_id)
        toolbelt_item_begin_alpha_action_event_handler(item_group);
}

void turn_fire_off(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    agent_fire_tick[agent_id] = 0;

    if (!agent_fire_on[agent_id]) return;

    agent_fire_on[agent_id] = false;

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemGroup item_group;
    if (item_id == NULL_ITEM) item_group = IG_NONE;
    else item_group = Item::get_item_group(item_id);
    if (ClientState::playerAgent_state.agent_id == agent_id)
    {
        toolbelt_item_end_alpha_action_event_handler(item_group);
        // force stop any continuous animations
        // TODO -- continuous actions from dat
        if (item_group == IG_MINING_LASER ||
			item_group == IG_MELEE_WEAPON ||
			item_group == IG_NONE ||
			item_group == IG_SHOVEL ||
			item_group == IG_NANITE_COIN || 
			item_group == IG_RESOURCE ||
			item_group == IG_ERROR)
			Animations::stop_equipped_item_animation();
	}
}

bool toolbelt_item_begin_alpha_action()
{
    int agent_id = ClientState::playerAgent_state.agent_id;
    if (agent_id < 0 || agent_id >= AGENT_MAX) return false;
    ASSERT_VALID_AGENT_ID(agent_id);

    if (agent_fire_on[agent_id]) return false;
    turn_fire_on(agent_id);
    
    if (agent_id == ClientState::playerAgent_state.agent_id)
    {
		ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
		int item_type = Item::get_item_type(item_id);
		int item_group = IG_NONE;
		if (item_type == NULL_ITEM_TYPE) item_group = IG_MELEE_WEAPON;
		else item_group = Item::get_item_group_for_type(item_type);
		bool continuous = false; // TODO -- load from dat
		if (item_group == IG_MINING_LASER ||
			item_group == IG_MELEE_WEAPON ||
			item_group == IG_NONE ||
			item_group == IG_SHOVEL ||
			item_group == IG_NANITE_COIN || 
			item_group == IG_RESOURCE ||
			item_group == IG_ERROR)
			continuous = true;
		Animations::begin_equipped_item_animation(item_type, continuous);
	}
	
    return true;
}

bool toolbelt_item_end_alpha_action()
{    
    // stop advancing fire tick
    int agent_id = ClientState::playerAgent_state.agent_id;
    if (agent_id < 0 || agent_id >= AGENT_MAX) return false;
    ASSERT_VALID_AGENT_ID(agent_id);

    if (!agent_fire_on[agent_id]) return false;
    turn_fire_off(agent_id);

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemGroup item_group;
    if (item_id == NULL_ITEM) item_group = IG_NONE;
    else item_group = Item::get_item_group(item_id);
    
    if (agent_id == ClientState::playerAgent_state.agent_id)
    {
		//int item_type = Item::get_item_type(item_id);
		Animations::stop_equipped_item_animation();
	}

    switch (item_group)
    {
        case IG_ERROR:
        case IG_RESOURCE:
        case IG_MELEE_WEAPON:
        case IG_NANITE_COIN:
        case IG_SHOVEL:
        case IG_NONE:
            return true;

        case IG_MINING_LASER:
            return true;

        case IG_CONSUMABLE:
        case IG_PLACER:
        case IG_HITSCAN_WEAPON:
        case IG_GRENADE_LAUNCHER:
            return false;   // nothing happened. they arent click-and-hold
        default:
            return false;    // the default action is click once
    }
    return true;
}

void toolbelt_item_begin_alpha_action_event_handler(ItemGroup item_group)
{
    switch (item_group)
    {
        case IG_MINING_LASER:
            ClientState::playerAgent_state.action.begin_mining_laser();
            break;

        case IG_ERROR:
        case IG_RESOURCE:
        case IG_MELEE_WEAPON:
        case IG_NANITE_COIN:
        case IG_SHOVEL:
        case IG_NONE:

        case IG_CONSUMABLE:
        case IG_PLACER:
        case IG_HITSCAN_WEAPON:
        case IG_GRENADE_LAUNCHER:
        default:
            break;    // the default action is click once
    }
}

void toolbelt_item_end_alpha_action_event_handler(ItemGroup item_group)
{
    switch (item_group)
    {
        case IG_MINING_LASER:
            ClientState::playerAgent_state.action.end_mining_laser();
            break;

        case IG_ERROR:
        case IG_RESOURCE:
        case IG_MELEE_WEAPON:
        case IG_NANITE_COIN:
        case IG_SHOVEL:
        case IG_NONE:

        case IG_CONSUMABLE:
        case IG_PLACER:
        case IG_HITSCAN_WEAPON:
        case IG_GRENADE_LAUNCHER:
        default:
            break;
    }
}

bool toolbelt_item_beta_action()
{
    // open any inventories in range

    using ClientState::playerAgent_state;
    int container_id = playerAgent_state.facing_container();
    if (container_id == NULL_CONTAINER) return false;

    bool opened = ItemContainer::open_container(container_id);
    if (!opened)
    {
        const char msg[] = "This container is locked.";
        chat_client->send_system_message((char*)msg);
    }
    return opened;
}

bool toolbelt_item_reload_action()
{
    return true;
}

} // Toolbelt
