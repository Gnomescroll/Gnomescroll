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
    Toolbelt::toolbelt_beta_action_CtoS::register_server_packet();

    // toolbelt actions (to client)
    Toolbelt::toolbelt_set_active_item_StoC::register_client_packet();
    Toolbelt::toolbelt_item_beta_action_StoC::register_client_packet();
    Toolbelt::toolbelt_item_begin_alpha_action_StoC::register_client_packet();
    Toolbelt::toolbelt_item_end_alpha_action_StoC::register_client_packet();
}

/* Miscellaneous Events */

void remove_agent(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;

    turn_fire_off(agent_id);

    IF_ASSERT(agent_selected_type != NULL)
        agent_selected_type[agent_id] = NULL_ITEM_TYPE;

    #if DC_SERVER
    IF_ASSERT(agent_selected_item != NULL) 
        agent_selected_item[agent_id] = NULL_ITEM;
    IF_ASSERT(agent_selected_slot != NULL)
        agent_selected_slot[agent_id] = 0;
    #endif
}

void agent_died(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;

    turn_fire_off(agent_id);
}

int get_agent_selected_item_type(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_ITEM_TYPE;
    IF_ASSERT(agent_selected_type == NULL) return NULL_ITEM_TYPE;
    
    return agent_selected_type[agent_id];
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

void tick_item(AgentID agent_id, ItemID item_id, int item_type)
{
    if (!item_is_click_and_hold(item_type))
        turn_fire_off(agent_id);

    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    tick(agent_id, item_id, item_type);
}

void trigger_item(AgentID agent_id, ItemID item_id, int item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
}

void trigger_item_beta(AgentID agent_id, ItemID item_id, int item_type)
{
    triggerItem trigger = get_trigger_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
}
#endif

#if DC_CLIENT
void tick_item(AgentID agent_id, int item_type)
{
    if (!item_is_click_and_hold(item_type))
        turn_fire_off(agent_id);
        
    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    tick(agent_id, item_type);    
}

void tick_local_item(ItemID item_id, int item_type)
{
    if (!item_is_click_and_hold(item_type))
        turn_fire_off(ClientState::playerAgent_state.agent_id);

    tickLocalItem tick = get_tick_local_item_fn(item_type);
    if (tick == NULL) return;
    tick(item_id, item_type);
}

void trigger_item(AgentID agent_id, int item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_type);
}

void trigger_item_beta(AgentID agent_id, int item_type)
{
    triggerItem trigger = get_trigger_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_type);
}

void trigger_local_item(ItemID item_id, int item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(item_id, item_type);

    if (!item_is_click_and_hold(item_type))
        turn_fire_off(ClientState::playerAgent_state.agent_id);
}

bool trigger_local_item_beta(ItemID item_id, int item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_beta_fn(item_type);
    if (trigger == NULL) return false;
    trigger(item_id, item_type);
    return true;
}

void begin_local_item(int item_type)
{
    beginLocalItem begin = get_begin_local_item_fn(item_type);
    if (begin == NULL) return;
    begin(item_type);
}

void end_local_item(int item_type)
{
    endLocalItem end = get_end_local_item_fn(item_type);
    if (end == NULL) return;
    end(item_type);
}

void begin_item(AgentID agent_id, int item_type)
{
    beginItem begin = get_begin_item_fn(item_type);
    if (begin == NULL) return;
    begin(agent_id, item_type);
}

void end_item(AgentID agent_id, int item_type)
{
    endItem end = get_end_item_fn(item_type);
    if (end == NULL) return;
    end(agent_id, item_type);
}
#endif


void tick()
{
    #if DC_SERVER
    GS_ASSERT(agent_selected_item != NULL);
    if (agent_selected_item == NULL) return;
    #endif
    
    GS_ASSERT(agent_selected_type != NULL);
    GS_ASSERT(agent_fire_tick     != NULL);
    GS_ASSERT(agent_fire_on       != NULL);
    if (agent_selected_type == NULL) return;
    if (agent_fire_tick == NULL) return;
    if (agent_fire_on == NULL) return;

    #if DC_CLIENT
    update_selected_item_type();
    int local_agent_id = ClientState::playerAgent_state.agent_id;
    ItemID local_item_id = ItemContainer::get_toolbelt_item(selected_slot);
    #endif
    
    // here's where i want to add x jetpack particles per frame i think.... sounds probably based on events
    for (int i=0; i<MAX_AGENTS; i++)
    {
		//Agents::agent_list[i].state

	}
    
    // increment fire ticks if weapon down
    for (int i=0; i<MAX_AGENTS; i++)
    {
        if (!agent_fire_on[i]) continue;

        #if DC_SERVER
        // check that selected_item matches selected_type here
        ItemID item_id = agent_selected_item[i];
        GS_ASSERT(agent_selected_type[i] == Item::get_item_type(item_id));
        #endif

        GS_ASSERT(Agents::agent_list->objects[i].id != Agents::agent_list->null_id); // agent should exist, if fire is on
        int item_type = agent_selected_type[i];
        if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;

        int fire_rate = Item::get_item_fire_rate(item_type);
        GS_ASSERT(fire_rate >= 1);
        if (fire_rate < 1) fire_rate = 1;
        
        if (agent_fire_tick[i] % fire_rate == 0)
        {
            #if DC_CLIENT
            if (local_agent_id == i)
                trigger_local_item(local_item_id, item_type);
            else trigger_item((AgentID)i, item_type);
            #endif
            
            #if DC_SERVER
            trigger_item((AgentID)i, item_id, item_type);
            #endif
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
        
        agent_fire_tick[i]++;
    }
}

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

int get_selected_item_type()
{
    GS_ASSERT(agent_selected_type != NULL);
    if (agent_selected_type == NULL) return NULL_ITEM_TYPE;
    AgentID agent_id = ClientState::playerAgent_state.agent_id;
    IF_ASSERT(!isValid(agent_id)) return NULL_ITEM_TYPE;
    return agent_selected_type[agent_id];
}

// there are edge cases where the server sets the item without client consent
// in the selected slot
// the item type needs to be periodically updated to ensure it is correct
void update_selected_item_type()
{
    AgentID agent_id = ClientState::playerAgent_state.agent_id;
    IF_ASSERT(!isValid(agent_id)) return;

    int item_type = NULL_ITEM_TYPE;
    ItemContainer::ItemContainer* toolbelt = NULL;
    if (toolbelt_id != NULL_CONTAINER) toolbelt =
        (ItemContainer::ItemContainer*)ItemContainer::get_container(toolbelt_id);
    if (toolbelt != NULL) item_type =
        Item::get_item_type(toolbelt->get_item(selected_slot));
    if (agent_selected_type[agent_id] == item_type) return;
    turn_fire_off(agent_id);
    agent_selected_type[agent_id] = item_type;
    Animations::stop_equipped_item_animation();
}

void assign_toolbelt(int container_id)
{
    toolbelt_id = container_id;
}

void toolbelt_item_selected_event(int container_id, int slot)
{
    if (selected_slot == slot) return;
    // dont check for death here
    // let them switch selected
    // if we want to prevent this, we need to have the UI also check for dead
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(container_id == toolbelt_id);
    if (container_id == NULL_CONTAINER || container_id != toolbelt_id) return;

    // cancel current triggers
    left_trigger_up_event();
    right_trigger_up_event();
    
    // update selected item
    selected_slot = slot;
    send_set_slot_packet(slot);
    update_selected_item_type();

    // stop any animations
    Animations::stop_equipped_item_animation();
}

void left_trigger_down_event()
{
    class Agent* you = ClientState::playerAgent_state.you();
    if (you == NULL || you->status.dead) return;
    bool something_happened = toolbelt_item_begin_alpha_action();
    if (something_happened) send_begin_alpha_action_packet();
}

void left_trigger_up_event()
{
    class Agent* you = ClientState::playerAgent_state.you();
    if (you == NULL || you->status.dead) return;
    bool something_happened = toolbelt_item_end_alpha_action();
    if (something_happened) send_end_alpha_action_packet();
}

void right_trigger_down_event()
{
    class Agent* you = ClientState::playerAgent_state.you();
    if (you == NULL || you->status.dead) return;
    bool something_happened = toolbelt_item_beta_action();
    if (something_happened) send_beta_action_packet();
}

void right_trigger_up_event()
{
    class Agent* you = ClientState::playerAgent_state.you();
    if (you == NULL || you->status.dead) return;
}

} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

void update_toolbelt_items()
{
    GS_ASSERT(agent_selected_type != NULL);
    GS_ASSERT(agent_selected_item != NULL);
    GS_ASSERT(agent_selected_slot != NULL);
    if (agent_selected_type == NULL) return;
    if (agent_selected_item == NULL) return;
    if (agent_selected_slot == NULL) return;
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
    
    int max = ItemContainer::get_container_max_slots(AGENT_TOOLBELT);
    GS_ASSERT(max > 0);
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
    agent_selected_item[agent_id] = NULL_ITEM;
    agent_selected_type[agent_id] = NULL_ITEM_TYPE;
}

} // Toolbelt
#endif
