#include "client.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <item/toolbelt/_interface.hpp>
#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/_interface.hpp>
#include <chat/_interface.hpp>
#include <item/toolbelt/net/CtoS.hpp>

namespace Toolbelt
{

void turn_fire_on(AgentID agent_id)
{
    IF_ASSERT(agent_fire_on == NULL) return;
    IF_ASSERT(agent_fire_cooldown == NULL) return;
    IF_ASSERT(!isValid(agent_id)) return;

    if (agent_fire_on[agent_id]) return;

    ItemType item_type = get_agent_selected_item_type(agent_id);
    if (ClientState::player_agent.agent_id == agent_id)
        toolbelt_item_begin_local_alpha_action_event_handler(item_type);
    else
        toolbelt_item_begin_alpha_action_event_handler(agent_id, item_type);

    agent_fire_cooldown[agent_id] = 0;
    agent_fire_on[agent_id] = true;
}

void turn_fire_off(AgentID agent_id)
{
    IF_ASSERT(click_and_hold == NULL) return;
    IF_ASSERT(agent_fire_on == NULL) return;
    IF_ASSERT(agent_fire_cooldown == NULL) return;
    IF_ASSERT(!isValid(agent_id)) return;

    if (!agent_fire_on[agent_id]) return;

    ItemType item_type = get_agent_selected_item_type(agent_id);
    if (ClientState::player_agent.agent_id == agent_id)
        toolbelt_item_end_local_alpha_action_event_handler(item_type);
    else
    {
        toolbelt_item_end_alpha_action_event_handler(agent_id, item_type);
        return;
    }

    GS_ASSERT(get_item_click_and_hold_behaviour(item_type) != CLICK_HOLD_NEVER);
    Animations::stop_equipped_item_animation();

    agent_fire_on[agent_id] = false;
}

// returns true if an event was or should be triggered
bool toolbelt_item_begin_alpha_action()
{
    IF_ASSERT(click_and_hold == NULL) return false;
    IF_ASSERT(agent_fire_on == NULL) return false;

    AgentID agent_id = ClientState::player_agent.agent_id;
    IF_ASSERT(!isValid(agent_id)) return false;

    if (agent_fire_cooldown[agent_id] > 0) return false;
    if (agent_fire_on[agent_id]) return false;

    turn_fire_on(agent_id);

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemType item_type = Item::get_item_type(item_id);
    ClickAndHoldBehaviour cnh = get_item_click_and_hold_behaviour(item_type);
    GS_ASSERT(cnh != CLICK_HOLD_NEVER);
    // For now, the only CLICK_HOLD_ALWAYS weapons have their own animation
    // i.e. the mining lasers.
    if (cnh == CLICK_HOLD_SOMETIMES)
        Animations::begin_equipped_item_animation(item_type, true);
    return true;
}

// returns true if an event was or should be triggered
bool toolbelt_item_end_alpha_action()
{
    IF_ASSERT(agent_fire_on == NULL) return false;
    IF_ASSERT(click_and_hold == NULL) return false;

    AgentID agent_id = ClientState::player_agent.agent_id;
    IF_ASSERT(!isValid(agent_id)) return false;

    if (!agent_fire_on[agent_id]) return false;
    turn_fire_off(agent_id);

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemType item_type = Item::get_item_type(item_id);
    GS_ASSERT(get_item_click_and_hold_behaviour(item_type) != CLICK_HOLD_NEVER);

    Animations::stop_equipped_item_animation();
    return true;
}

bool toolbelt_item_alpha_action()
{
    IF_ASSERT(agent_fire_on == NULL) return false;
    IF_ASSERT(agent_selected_type == NULL) return false;
    AgentID agent_id = ClientState::player_agent.agent_id;
    IF_ASSERT(!isValid(agent_id)) return false;

    if (agent_fire_on[agent_id]) return false;
    if (agent_fire_cooldown[agent_id] > 0) return false;
    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemType item_type = fist_item_type;
    if (item_id != NULL_ITEM)
        item_type = Item::get_item_type(item_id);
    IF_ASSERT(!isValid(item_type)) return false;

    agent_fire_cooldown[agent_id] = Item::get_item_fire_rate(item_type);
    Animations::begin_equipped_item_animation(item_type, false);

    return trigger_local_item(item_id, item_type);
}

void toolbelt_item_begin_local_alpha_action_event_handler(ItemType item_type)
{
    begin_local_item(item_type);
}

void toolbelt_item_end_local_alpha_action_event_handler(ItemType item_type)
{
    end_local_item(item_type);
}

void toolbelt_item_begin_alpha_action_event_handler(AgentID agent_id, ItemType item_type)
{
    GS_ASSERT(agent_id != ClientState::player_agent.agent_id); // use local
    begin_item(agent_id, item_type);
}

void toolbelt_item_end_alpha_action_event_handler(AgentID agent_id, ItemType item_type)
{
    GS_ASSERT(agent_id != ClientState::player_agent.agent_id); // use local
    end_item(agent_id, item_type);
}

static bool beta_scan_world()
{   // check for things in world that respond to beta action
    if (agent_camera == NULL) return false;

    float range = BETA_WORLD_EFFECT_RANGE - 0.2f;   // reduce margin to correct for discrepancy btwn client and server camera position
    //float range = BETA_WORLD_EFFECT_RANGE;   // reduce margin to correct for discrepancy btwn client and server camera position

    Vec3 pos = agent_camera->get_position();

    using ClientState::hitscan;
    if (hitscan.distance > range) return false;

    ItemContainerID container_id = NULL_CONTAINER;
    switch (hitscan.type)
    {
        case HITSCAN_TARGET_VOXEL:
            if (hitscan.voxel_target.entity_type == OBJECT_AGENT_SPAWNER)
            {
                choose_spawner_CtoS msg;
                msg.spawner_id = hitscan.voxel_target.entity_id;
                msg.send();
                return true;
            }
            return false;

        case HITSCAN_TARGET_BLOCK:
            if (!ItemContainer::container_block_in_range_of(pos, hitscan.block_position))
                return false;
            container_id = t_map::get_block_item_container(hitscan.block_position);
            if (container_id != NULL_CONTAINER)
            {
                bool opened = ItemContainer::open_container(container_id);
                if (opened) return true;
                Chat::send_system_message("This container is locked.");
                return false;
            }
            return false;

        case HITSCAN_TARGET_MECH:
            // TODO -- configure mechs with beta-actions, and look up that
            // callback
            return false;

        case HITSCAN_TARGET_NONE:
            return false;
    }
    return false;
}

// returns true if an event was or should be triggered
bool toolbelt_item_beta_action()
{
    IF_ASSERT(agent_fire_on == NULL) return false;
    IF_ASSERT(agent_selected_type == NULL) return false;

    AgentID agent_id = ClientState::player_agent.agent_id;
    IF_ASSERT(!isValid(agent_id)) return false;

    if (agent_fire_on[agent_id]) return false;

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemType item_type = NULL_ITEM_TYPE;
    if (item_id == NULL_ITEM)
        item_type = fist_item_type;
    else
        item_type = Item::get_item_type(item_id);
    IF_ASSERT(!isValid(item_type)) return false;

    bool triggered = trigger_local_item_beta(item_id, item_type);
    if (triggered) return true;

    // if no beta actions are defined, check against world items
    return beta_scan_world();
}

/* Packets */

void send_set_slot_packet(int slot)
{
    int max = ItemContainer::get_container_max_slots(ItemContainer::name::toolbelt);
    IF_ASSERT(max <= 0) return;
    IF_ASSERT(slot < 0 || slot >= max) return;
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

void send_alpha_action_packet()
{
    toolbelt_alpha_action_CtoS msg;
    msg.send();
}

void send_beta_action_packet()
{
    toolbelt_beta_action_CtoS msg;
    msg.send();
}

} // Toolbelt
