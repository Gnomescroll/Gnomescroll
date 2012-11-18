#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
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
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return;
    GS_ASSERT(agent_fire_tick != NULL);
    if (agent_fire_tick == NULL) return;
    
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    agent_fire_tick[agent_id] = 0;
    if (agent_fire_on[agent_id]) return;
    agent_fire_on[agent_id] = true;

    int item_type = get_agent_selected_item_type(agent_id);
    if (ClientState::playerAgent_state.agent_id == agent_id)
        toolbelt_item_begin_local_alpha_action_event_handler(item_type);
    else
        toolbelt_item_begin_alpha_action_event_handler(agent_id, item_type);
}

void turn_fire_off(AgentID agent_id)
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

    int item_type = get_agent_selected_item_type(agent_id);
    if (ClientState::playerAgent_state.agent_id == agent_id)
        toolbelt_item_end_local_alpha_action_event_handler(item_type);
    else
    {
        toolbelt_item_end_alpha_action_event_handler(agent_id, item_type);
        return;
    }

    if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;
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

    AgentID agent_id = ClientState::playerAgent_state.agent_id;
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

    AgentID agent_id = ClientState::playerAgent_state.agent_id;
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

void toolbelt_item_begin_local_alpha_action_event_handler(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;
    begin_local_item(item_type);
}

void toolbelt_item_end_local_alpha_action_event_handler(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;
    end_local_item(item_type);
}

void toolbelt_item_begin_alpha_action_event_handler(AgentID agent_id, int item_type)
{
    GS_ASSERT(agent_id != ClientState::playerAgent_state.agent_id); // use local
    if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;
    begin_item(agent_id, item_type);
}

void toolbelt_item_end_alpha_action_event_handler(AgentID agent_id, int item_type)
{
    GS_ASSERT(agent_id != ClientState::playerAgent_state.agent_id); // use local
    if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;
    end_item(agent_id, item_type);
}

static bool beta_scan_world()
{   // check for things in world that respond to beta action
    using ClientState::playerAgent_state;
    if (agent_camera == NULL) return false;
    
    float range = BETA_WORLD_EFFECT_RANGE - 0.2f;   // reduce margin to correct for discrepancy btwn client and server camera position
    //float range = BETA_WORLD_EFFECT_RANGE;   // reduce margin to correct for discrepancy btwn client and server camera position

    Vec3 pos = playerAgent_state.camera_position();
    Vec3 look = agent_camera->forward_vector();

    class Voxel_hitscan_target target;
    float vox_distance;
    float collision_point[3];
    int block_pos[3];
    int side[3];
    CubeID tile = EMPTY_CUBE;
    float block_distance;

    HitscanTargetTypes target_type =
        Hitscan::hitscan_against_world(
            pos, look, playerAgent_state.agent_id, OBJECT_AGENT,
            &target, &vox_distance, collision_point,
            block_pos, side, &tile, &block_distance
        );

    int container_id = NULL_CONTAINER;
    switch (target_type)
    {
        case HITSCAN_TARGET_VOXEL:
            if (vox_distance > range) return false;
            if (target.entity_type == OBJECT_AGENT_SPAWNER)
            {
                choose_spawner_CtoS msg;
                msg.spawner_id = target.entity_id;
                msg.send();
                return true;
            }
            return false;
        
        case HITSCAN_TARGET_BLOCK:
            if (!ItemContainer::container_block_in_range_of(pos, block_pos)) return false; 
            container_id = t_map::get_block_item_container(block_pos[0], block_pos[1], block_pos[2]);
            if (container_id != NULL_CONTAINER)
            {
                bool opened = ItemContainer::open_container(container_id);
                if (opened) return true;
                Chat::send_system_message("This container is locked.");
                return false;
            }
            return false;

        default:
            return false;
    }
    return false;
}

// returns true if an event was or should be triggered
bool toolbelt_item_beta_action()
{
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return false;
    GS_ASSERT(agent_selected_type != NULL)
    if (agent_selected_type == NULL) return false;

    AgentID agent_id = ClientState::playerAgent_state.agent_id;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;

    if (agent_fire_on[agent_id]) return false;

    ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);
    int item_type = NULL_ITEM_TYPE;
    if (item_id == NULL_ITEM) item_type = fist_item_type;
    else item_type = Item::get_item_type(item_id);
    GS_ASSERT(item_type != NULL_ITEM_TYPE)
    if (item_type < 0 || item_type >= MAX_ITEM_TYPES) return false;
    
    bool triggered = trigger_local_item_beta(item_id, item_type);
    if (triggered) return true;

    // if no beta actions are defined, check against world items
    return beta_scan_world();
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
