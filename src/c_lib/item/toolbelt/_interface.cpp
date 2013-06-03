#include "_interface.hpp"

#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/config.hpp>
#include <item/toolbelt/config/_state.hpp>
#include <item/container/config/_interface.hpp>
#include <agent/_interface.hpp>

// Common
namespace Toolbelt
{

/* Init / Teardown */

void init()
{
    init_state();
    init_config();
}

void teardown()
{
    teardown_state();
    teardown_config();
}

void init_packets()
{
    // toolbelt actions (to server)
    Toolbelt::toolbelt_set_slot_CtoS::register_server_packet();
    Toolbelt::toolbelt_begin_alpha_action_CtoS::register_server_packet();
    Toolbelt::toolbelt_end_alpha_action_CtoS::register_server_packet();
    Toolbelt::toolbelt_alpha_action_CtoS::register_server_packet();
    Toolbelt::toolbelt_beta_action_CtoS::register_server_packet();

    // toolbelt actions (to client)
    Toolbelt::toolbelt_set_active_item_StoC::register_client_packet();
    Toolbelt::toolbelt_item_beta_action_StoC::register_client_packet();
    Toolbelt::toolbelt_item_alpha_action_StoC::register_client_packet();
    Toolbelt::toolbelt_item_begin_alpha_action_StoC::register_client_packet();
    Toolbelt::toolbelt_item_end_alpha_action_StoC::register_client_packet();
}

/* Miscellaneous Events */

void remove_agent(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;

    turn_fire_off(agent_id);

    GS_ASSERT(agent_selected_type != NULL);
    if (agent_selected_type != NULL)
        agent_selected_type[agent_id] = NULL_ITEM_TYPE;

    #if DC_SERVER
    GS_ASSERT(agent_selected_item != NULL)
    if (agent_selected_item != NULL)
        agent_selected_item[agent_id] = NULL_ITEM;
    GS_ASSERT(agent_selected_slot != NULL)
    if (agent_selected_slot != NULL)
        agent_selected_slot[agent_id] = 0;
    #endif
}

void agent_died(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    turn_fire_off(agent_id);
}

ItemType get_agent_selected_item_type(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_ITEM_TYPE;
    IF_ASSERT(agent_selected_type == NULL) return NULL_ITEM_TYPE;
    ItemType type = agent_selected_type[agent_id];
    if (type == NULL_ITEM_TYPE)
        type = fist_item_type;
    return type;
}

/* Trigger entry points */
// Most of these do not need declarations exported in the header

#if DC_SERVER

void agent_quit(AgentID agent_id)
{   // It might be the same as agent_died, but they should not be mixed
    // Death has penalties; leaving the server should not
    IF_ASSERT(!isValid(agent_id)) return;
    turn_fire_off(agent_id);
}

void tick_item(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    tick(agent_id, item_id, item_type);
}

void trigger_item(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
}

void trigger_item_beta(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    triggerItem trigger = get_trigger_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
}
#endif

#if DC_CLIENT
void tick_item(AgentID agent_id, ItemType item_type)
{
    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    tick(agent_id, item_type);
}

void tick_local_item(ItemID item_id, ItemType item_type)
{
    tickLocalItem tick = get_tick_local_item_fn(item_type);
    if (tick == NULL) return;
    tick(item_id, item_type);
}

void trigger_item(AgentID agent_id, ItemType item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_type);
}

void trigger_item_beta(AgentID agent_id, ItemType item_type)
{
    triggerItem trigger = get_trigger_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_type);
}

bool trigger_local_item(ItemID item_id, ItemType item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_fn(item_type);
    if (trigger == NULL) return false;
    trigger(item_id, item_type);
    return true;
}

bool trigger_local_item_beta(ItemID item_id, ItemType item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_beta_fn(item_type);
    if (trigger == NULL) return false;
    trigger(item_id, item_type);
    return true;
}

void begin_local_item(ItemType item_type)
{
    beginLocalItem begin = get_begin_local_item_fn(item_type);
    if (begin == NULL) return;
    begin(item_type);
}

void end_local_item(ItemType item_type)
{
    endLocalItem end = get_end_local_item_fn(item_type);
    if (end == NULL) return;
    end(item_type);
}

void begin_item(AgentID agent_id, ItemType item_type)
{
    beginItem begin = get_begin_item_fn(item_type);
    if (begin == NULL) return;
    begin(agent_id, item_type);
}

void end_item(AgentID agent_id, ItemType item_type)
{
    endItem end = get_end_item_fn(item_type);
    if (end == NULL) return;
    end(agent_id, item_type);
}
#endif


void tick()
{
    #if DC_SERVER
    IF_ASSERT(agent_selected_item == NULL) return;
    #endif

    IF_ASSERT(agent_fire_cooldown == NULL) return;
    IF_ASSERT(agent_selected_type == NULL) return;
    IF_ASSERT(agent_fire_on == NULL) return;

    #if DC_CLIENT
    update_selected_item_type();
    ItemID local_item_id = ItemContainer::get_toolbelt_item(selected_slot);
    ItemType local_item_type = Item::get_item_type(local_item_id);
    if (local_item_type == NULL_ITEM_TYPE)
        local_item_type = fist_item_type;
    AgentID local_agent_id = ClientState::player_agent.agent_id;
    if (!isValid(local_agent_id)) return;
    #endif

    // increment fire ticks if weapon down
    for (int i=0; i<MAX_AGENTS; i++)
    {
        agent_fire_cooldown[i] = GS_MAX(agent_fire_cooldown[i] - 1, 0);
        if (!agent_fire_on[i]) continue;

        #if DC_SERVER
        // check that selected_item matches selected_type here
        ItemID item_id = agent_selected_item[i];
        GS_ASSERT(agent_selected_type[i] == Item::get_item_type(item_id));
        #endif

        // agent should exist, if fire is on
        GS_ASSERT(Agents::agent_list->objects[i].id != Agents::agent_list->null_id);

        ItemType item_type = agent_selected_type[i];
        if (item_type == NULL_ITEM_TYPE)
            item_type = fist_item_type;

        if (agent_fire_cooldown[i] == 0)
        {
            #if DC_CLIENT
            if (local_agent_id == i)
                trigger_local_item(local_item_id, item_type);
            #endif

            #if DC_SERVER
            trigger_item((AgentID)i, item_id, item_type);
            #endif

            int fire_rate = Item::get_item_fire_rate(item_type);
            IF_ASSERT(fire_rate < 1) fire_rate = 1;
            agent_fire_cooldown[i] = fire_rate;
        }

        #if DC_CLIENT
        if (local_agent_id == i)
            tick_local_item(local_item_id, item_type);
        else
            tick_item((AgentID)i, item_type);
        #endif

        #if DC_SERVER
        tick_item((AgentID)i, item_id, item_type);
        #endif
    }

    #if DC_CLIENT
    // If we're currently click-and-hold firing on terrain, shut it off if
    // we are no longer looking at terrain
    if (!single_trigger && get_item_click_and_hold_behaviour(local_item_type) == CLICK_HOLD_SOMETIMES)
    {
        if (agent_fire_on[local_agent_id])
        {
            if (!ClientState::player_agent.pointing_at_terrain(local_item_type))
                if (toolbelt_item_end_alpha_action())
                    send_end_alpha_action_packet();
        }
        else if (holding)
        {
            if (ClientState::player_agent.pointing_at_terrain(local_item_type))
                if (toolbelt_item_begin_alpha_action())
                    send_begin_alpha_action_packet();
        }
    }

    charging = false;
    if (holding && single_trigger &&
        get_item_charge_behaviour(local_item_type) != CHARGE_NEVER)
    {
        holding_tick++;
        if (holding_tick >= CHARGE_THRESHOLD)
            charging = true;
    }
    else
        holding_tick = 0;
    #endif
}

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

ItemType get_selected_item_type()
{
    IF_ASSERT(agent_selected_type == NULL) return fist_item_type;
    AgentID agent_id = ClientState::player_agent.agent_id;
    if (!isValid(agent_id)) return fist_item_type;
    ItemType type = agent_selected_type[agent_id];
    if (type == NULL_ITEM_TYPE)
        type = fist_item_type;
    return type;
}

// there are edge cases where the server sets the item without client consent
// in the selected slot
// the item type needs to be periodically updated to ensure it is correct
void update_selected_item_type()
{
    AgentID agent_id = ClientState::player_agent.agent_id;
    if (!isValid(agent_id)) return;

    ItemType item_type = NULL_ITEM_TYPE;
    ItemContainer::ItemContainerInterface* toolbelt = NULL;
    if (toolbelt_id != NULL_CONTAINER)
        toolbelt = ItemContainer::get_container(toolbelt_id);
    if (toolbelt != NULL)
        item_type = Item::get_item_type(toolbelt->get_item(selected_slot));
    set_agent_selected_item_type(agent_id, item_type);
}

void set_agent_selected_item_type(AgentID agent_id, ItemType item_type)
{
    IF_ASSERT(!isValid(agent_id)) return;
    if (agent_selected_type[agent_id] == item_type)
        return;
    turn_fire_off(agent_id);
    agent_selected_type[agent_id] = item_type;
    if (agent_id == ClientState::player_agent.agent_id)
    {
        agent_camera->unzoom();
        Animations::stop_equipped_item_animation();
    }
}

void assign_toolbelt(ItemContainerID container_id)
{
    toolbelt_id = container_id;
}

void toolbelt_item_selected_event(ItemContainerID container_id, int slot)
{
    if (selected_slot == slot) return;
    // dont check for death here
    // let them switch selected
    // if we want to prevent this, we need to have the UI also check for dead
    IF_ASSERT(!isValid(container_id)) return;
    IF_ASSERT(container_id != toolbelt_id) return;

    // cancel current triggers
    left_trigger_up_event();
    right_trigger_up_event();

    // update selected item
    selected_slot = slot;
    send_set_slot_packet(slot);
    update_selected_item_type();
}

void left_trigger_down_event()
{
    holding = true;
    holding_tick = 0;

    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL || you->status.dead) return;


    ItemType item_type = get_selected_item_type();
    ClickAndHoldBehaviour cnh = get_item_click_and_hold_behaviour(item_type);
    ChargeBehaviour cb = get_item_charge_behaviour(item_type);
    single_trigger = (cnh == CLICK_HOLD_NEVER ||
                      (cnh == CLICK_HOLD_SOMETIMES &&
                       !ClientState::player_agent.pointing_at_terrain(item_type)));

    if (!single_trigger)
    {
        if (toolbelt_item_begin_alpha_action())
            send_begin_alpha_action_packet();
    }
    else if (cb == CHARGE_NEVER)
    {
        if (toolbelt_item_alpha_action())
            send_alpha_action_packet();
    }
}

void left_trigger_up_event()
{
    if (!holding) return;
    holding = false;

    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL || you->status.dead) return;

    ItemType item_type = get_selected_item_type();
    ChargeBehaviour cb = get_item_charge_behaviour(item_type);

    bool turn_off = !single_trigger;

    // If it's a CHARGE_ALWAYS and we had a single_trigger event, emit charged attack proportional to charging
    if (cb == CHARGE_ALWAYS && single_trigger)
    {   // unleash a charged attack (if holding_tick is past threshold)
        float charge_multiplier = 1.0f + get_charge_progress();

        if (toolbelt_item_alpha_action())
        {
            printf("Charged attack. Multiplier: %0.2f\n", charge_multiplier);
            send_alpha_action_packet();
            turn_off = false;
        }
    }

    if (turn_off)
    {
        if (toolbelt_item_end_alpha_action())
            send_end_alpha_action_packet();
    }

    single_trigger = false;
    holding_tick = 0;
}

void right_trigger_down_event()
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL || you->status.dead) return;
    if (toolbelt_item_beta_action())
        send_beta_action_packet();
}

void right_trigger_up_event()
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL || you->status.dead) return;
}

float get_charge_progress()
{
    return GS_MIN(holding_tick - CHARGE_THRESHOLD, CHARGE_MAX) / float(CHARGE_MAX);
}

} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

void update_toolbelt_items()
{
    IF_ASSERT(agent_selected_type == NULL) return;
    IF_ASSERT(agent_selected_item == NULL) return;
    IF_ASSERT(agent_selected_slot == NULL) return;
    // make sure agent_selected_item is current
    // if any discrepancies exist, send a set_selected_item packet
    for (int i=0; i<MAX_AGENTS; i++)
    {
        AgentID agent_id = (AgentID)i;
        int slot = agent_selected_slot[agent_id];
        GS_ASSERT(slot != NULL_SLOT);
        ItemID item_id = ItemContainer::get_agent_toolbelt_item(agent_id, slot);
        if (item_id == agent_selected_item[agent_id]) continue;
        turn_fire_off(agent_id);
        agent_selected_item[agent_id] = item_id;
        agent_selected_type[agent_id] = Item::get_item_type(item_id);
        broadcast_agent_set_active_item_packet(agent_id, agent_selected_type[agent_id]);
    }
}

ItemID get_agent_selected_item(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_ITEM;
    IF_ASSERT(agent_selected_item == NULL) return NULL_ITEM;

    return agent_selected_item[agent_id];
}

int get_agent_selected_slot(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_SLOT;
    IF_ASSERT(agent_selected_slot == NULL) return NULL_SLOT;

    return agent_selected_slot[agent_id];
}

bool set_agent_toolbelt_slot(AgentID agent_id, int slot)
{
    IF_ASSERT(!isValid(agent_id)) return false;

    int max = ItemContainer::get_container_max_slots(ItemContainer::name::toolbelt);
    IF_ASSERT(max <= 0) return false;
    IF_ASSERT(slot < 0 || slot >= max || slot == NULL_SLOT) return false;
    agent_selected_slot[agent_id] = slot;
    ItemID item_id = ItemContainer::get_agent_toolbelt_item(agent_id, slot);
    if (item_id == agent_selected_item[agent_id]) return false;
    agent_selected_item[agent_id] = item_id;
    agent_selected_type[agent_id] = Item::get_item_type(item_id);
    return true;
}

// TODO -- rewrite code so that it doesnt need to callback again here
// or add yet another configuration
// or just leave this special case
void use_block_placer(AgentID agent_id, ItemID placer_id)
{
    IF_ASSERT(!isValid(agent_id)) return;

    Item::Item* placer = Item::get_item(placer_id);
    if (placer == NULL) return;

    int stack_size = placer->stack_size;
    int remaining_stack_size = Item::consume_stack_item(placer->id);
    // force removal is already handled if the item gets destroyed
    if (remaining_stack_size > 0 && stack_size != remaining_stack_size)
        Item::send_item_state(placer->id);
}

void force_remove_selected_item(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    turn_fire_off(agent_id);
    if (agent_selected_item[agent_id] != NULL_ITEM)
    {
        GS_ASSERT(agent_selected_type[agent_id] != NULL_ITEM_TYPE);
        broadcast_agent_set_active_item_packet(agent_id, NULL_ITEM_TYPE);
    }
    agent_selected_item[agent_id] = NULL_ITEM;
    agent_selected_type[agent_id] = NULL_ITEM_TYPE;
}

} // Toolbelt
#endif
