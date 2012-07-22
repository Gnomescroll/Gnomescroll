#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <item/toolbelt/_interface.hpp>
#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/_interface.hpp>
#include <chat/interface.hpp>
#include <item/toolbelt/net/CtoS.hpp>

namespace Toolbelt
{

void turn_fire_on(int agent_id)
{
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return;
    GS_ASSERT(agent_fire_tick != NULL);
    if (agent_fire_tick == NULL) return;
    
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    agent_fire_tick[agent_id] = 0;
    if (agent_fire_on[agent_id]) return;
    agent_fire_on[agent_id] = true;

    if (ClientState::playerAgent_state.agent_id != agent_id) return;
    
    // local agent only:
    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    toolbelt_item_begin_alpha_action_event_handler(item_id);
}

void turn_fire_off(int agent_id)
{
    GS_ASSERT(click_and_hold != NULL);
    if (click_and_hold == NULL) return;
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return;
    GS_ASSERT(agent_fire_tick != NULL);
    if (agent_fire_tick == NULL) return;
    
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    agent_fire_tick[agent_id] = 0;
    if (!agent_fire_on[agent_id]) return;
    agent_fire_on[agent_id] = false;

    if (ClientState::playerAgent_state.agent_id != agent_id) return;
    
    // local agent only:
    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    toolbelt_item_end_alpha_action_event_handler(item_id);

    int item_type = NULL_ITEM_TYPE;
    if (item_id == NULL_ITEM) item_type = fist_item_type;
    else item_type = Item::get_item_type(item_id);
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type < 0 || item_type >= MAX_ITEMS) return;

    if (item_is_click_and_hold(item_type))
        Animations::stop_equipped_item_animation();
}

// returns true if an event was or should be triggered
bool toolbelt_item_begin_alpha_action()
{
    GS_ASSERT(click_and_hold != NULL);
    if (click_and_hold == NULL) return false;
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return false;

    int agent_id = ClientState::playerAgent_state.agent_id;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;

    if (agent_fire_on[agent_id]) return false;

    turn_fire_on(agent_id);

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    int item_type = Item::get_item_type(item_id);

    Animations::begin_equipped_item_animation(item_type, item_is_click_and_hold(item_type));
    return true;
}

// returns true if an event was or should be triggered
bool toolbelt_item_end_alpha_action()
{    
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return false;
    GS_ASSERT(click_and_hold != NULL);
    if (click_and_hold == NULL) return false;

    int agent_id = ClientState::playerAgent_state.agent_id;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;

    if (!agent_fire_on[agent_id]) return false;
    turn_fire_off(agent_id);

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    int item_type = Item::get_item_type(item_id);
    
    if (item_is_click_and_hold(item_type))
    {
        Animations::stop_equipped_item_animation();
        return true;
    }

    return false;
}

void toolbelt_item_begin_alpha_action_event_handler(ItemID item_id)
{
    int item_type = fist_item_type;
    if (item_id != NULL_ITEM) item_type = Item::get_item_type(item_id);
    begin_local_item(item_type);
}

void toolbelt_item_end_alpha_action_event_handler(ItemID item_id)
{
    int item_type = fist_item_type;
    if (item_id != NULL_ITEM) item_type = Item::get_item_type(item_id);
    end_local_item(item_type);
}

// returns true if an event was or should be triggered
bool toolbelt_item_beta_action()
{
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return false;
    GS_ASSERT(agent_selected_type != NULL)
    if (agent_selected_type == NULL) return false;

    int agent_id = ClientState::playerAgent_state.agent_id;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;

    if (agent_fire_on[agent_id]) return false;

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    int item_type = NULL_ITEM_TYPE;
    if (item_id == NULL_ITEM) item_type = fist_item_type;
    else item_type = Item::get_item_type(item_id);
    GS_ASSERT(item_type != NULL_ITEM_TYPE)
    if (item_type < 0 || item_type >= MAX_ITEMS) return false;
    
    bool triggered = trigger_local_item_beta(item_id, item_type);
    if (triggered) return true;

    // if no beta actions are defined,
    
    // open any inventories in range
    int container_id = ClientState::playerAgent_state.facing_container();
    if (container_id == NULL_CONTAINER) return false;

    bool opened = ItemContainer::open_container(container_id);
    if (!opened)
    {
        const char msg[] = "This container is locked.";
        chat_client->send_system_message(msg);
    }
    return opened;
}

/* Packets */

void send_set_slot_packet(int slot)
{
    int max = ItemContainer::get_container_max_slots(AGENT_TOOLBELT);
    GS_ASSERT(max > 0);
    GS_ASSERT(slot >= 0 && slot != NULL_SLOT && slot < max);
    if (slot < 0 || slot >= max) return;
    toolbelt_set_slot_CtoS msg;
    msg.slot = slot;
    msg.send();
}

void send_begin_alpha_action_packet()
{
    toolbelt_begin_alpha_action_CtoS msg;
    msg.send();
}

void send_end_alpha_action_packet()
{
    toolbelt_end_alpha_action_CtoS msg;
    msg.send();
}

void send_beta_action_packet()
{
    toolbelt_beta_action_CtoS msg;
    msg.send();
}

} // Toolbelt
