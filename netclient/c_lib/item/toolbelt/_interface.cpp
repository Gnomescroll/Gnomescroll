#include "_interface.hpp"

// Common
namespace Toolbelt
{

void init()
{
    assert(agent_selected_type == NULL);
    assert(agent_fire_tick     == NULL);
    assert(agent_fire_on       == NULL);

    agent_selected_type = (int*)malloc(AGENT_MAX * sizeof(int));
    for (int i=0; i<AGENT_MAX; agent_selected_type[i++] = NULL_ITEM_TYPE);
    agent_fire_tick = (int*)calloc(AGENT_MAX, sizeof(int));
    agent_fire_on   = (bool*)calloc(AGENT_MAX, sizeof(bool));
    
    #if DC_CLIENT
    #endif

    #if DC_SERVER
    assert(agent_selected_slot == NULL);
    assert(agent_selected_item == NULL);
    agent_selected_slot = (int*)calloc(AGENT_MAX, sizeof(int));
    agent_selected_item = (ItemID*)malloc(AGENT_MAX * sizeof(ItemID));
    for (int i=0; i<AGENT_MAX; agent_selected_item[i++] = NULL_ITEM);
    #endif
}

void teardown()
{
    if (agent_selected_type != NULL) free(agent_selected_type);
    if (agent_fire_tick != NULL)     free(agent_fire_tick);
    if (agent_fire_on   != NULL)     free(agent_fire_on);

    #if DC_CLIENT
    #endif
    
    #if DC_SERVER
    if (agent_selected_slot != NULL) free(agent_selected_slot);
    if (agent_selected_item != NULL) free(agent_selected_item);
    #endif
}

void tick()
{
    #if DC_SERVER
    assert(agent_selected_item != NULL);
    #endif
    assert(agent_selected_type != NULL);
    assert(agent_fire_tick     != NULL);
    assert(agent_fire_on       != NULL);

    #if DC_CLIENT
    int local_agent_id = ClientState::playerAgent_state.agent_id;
    #endif
    
    // increment fire ticks if weapon down
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (!agent_fire_on[i]) continue;
        int fire_rate = Item::get_item_fire_rate(agent_selected_type[i]);
        if (agent_fire_tick[i] % fire_rate == 0)
        {
            #if DC_CLIENT
            if (local_agent_id == i) trigger_local_agent_selected_item_type(agent_selected_type[i]);
            trigger_agent_selected_item_type(i, agent_selected_type[i]);
            #endif
            #if DC_SERVER
            trigger_agent_selected_item(i, agent_selected_item[i]);
            #endif
        }
        #if DC_CLIENT
        if (local_agent_id == i) tick_local_agent_selected_item_type(agent_selected_type[i]);
        else tick_agent_selected_item_type(i, agent_selected_type[i]);
        #endif
        #if DC_SERVER
        tick_agent_selected_item(i, agent_selected_item[i]);
        #endif
        agent_fire_tick[i]++;
    }
}

void remove_agent(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    #if DC_SERVER
    assert(agent_selected_item != NULL);
    #endif
    assert(agent_selected_type != NULL);
    assert(agent_fire_tick     != NULL);
    assert(agent_fire_on       != NULL);

    #if DC_SERVER
    agent_selected_item[agent_id] = NULL_ITEM;
    #endif
    agent_selected_type[agent_id] = NULL_ITEM_TYPE;
    agent_fire_tick[agent_id] = 0;
    agent_fire_on[agent_id] = false;
}

void agent_died(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    #if DC_SERVER
    assert(agent_selected_item != NULL);
    #endif
    assert(agent_selected_type != NULL);
    assert(agent_fire_tick     != NULL);
    assert(agent_fire_on       != NULL);

    agent_fire_tick[agent_id] = 0;
    agent_fire_on[agent_id] = false;
}

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

// there are edge cases where the server sets the item without client consent
// in the selected slot
// the item type needs to be periodically updated to ensure it is correct
void update_selected_item_type()
{
    int agent_id = ClientState::playerAgent_state.agent_id;
    if (agent_id < 0 || agent_id >= AGENT_MAX) return;
    int item_type = NULL_ITEM_TYPE;
    Item::ItemContainer* toolbelt = (Item::ItemContainer*)Item::get_container(toolbelt_id);
    if (toolbelt != NULL) item_type = Item::get_item_type(toolbelt->get_item(selected_slot));
    agent_selected_type[agent_id] = item_type;
}

// tick for all agents (including local)
// will play continuous animations/sounds
void tick_agent_selected_item_type(int agent_id, int item_type)
{
    if (Item::get_item_group_for_type(item_type) != IG_MINING_LASER) return;
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    a->event.tick_mining_laser();
}

// trigger for all agents (including local)
// will play animations/sounds
void trigger_agent_selected_item_type(int agent_id, int item_type)
{
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;

    //int item_group = Item::get_item_group_for_type(item_type);
    //if (item_group == IG_PLACER)
    //{
        //Item::ItemContainer* container = (Item::ItemContainer*)Item::get_container(toolbelt_id);
        //if (container == NULL) return;
        //ItemID item_id = container->get_item(selected_slot);
        //assert(item_id == NULL_ITEM);
        //// consume stack
        //printf("consume stack\n");
        //int stack_size = Item::get_stack_size(item_id) - 1;
        //if (stack_size < 1) stack_size = 1; // cap at 1, because we do not handle <=0 stack sizes even in UI
        //Item::set_ui_slot_stack_size(toolbelt_id, selected_slot, stack_size);
    //}
    //else if (item_group == IG_MINING_LASER)
    //{
        a->event.fired_mining_laser();
    //}
}

// tick for the local agent
void tick_local_agent_selected_item_type(int item_type)
{
    ClientState::playerAgent_state.action.tick_mining_laser();
    // modify predicted durability
    Item::ItemContainer* container = (Item::ItemContainer*)Item::get_container(toolbelt_id);
    if (container == NULL) return;
    ItemID item_id = container->get_item(selected_slot);
    Item::Item* item = Item::get_item(item_id);
    if (item == NULL) return;

    // consume durability
    int durability = item->durability - 1;
    if (durability < 0) durability = 0;
    Item::set_ui_slot_durability(toolbelt_id, selected_slot, durability);
}

// trigger for the local agent
// will send hitscan packets
void trigger_local_agent_selected_item_type(int item_type)
{
    ClientState::playerAgent_state.action.fire_mining_laser();
}

void assign_toolbelt(int container_id)
{
    toolbelt_id = container_id;
}

void toolbelt_item_selected_event(int container_id, int slot)
{
    // dont check for death here
    // let them switch selected
    // if we want to prevent this, we need to have the UI also check for dead
    if (container_id == NULL_CONTAINER || container_id != toolbelt_id) return;
    // update selected item
    selected_slot = slot;
    send_set_slot_packet(slot);
    update_selected_item_type();
}

void left_trigger_down_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
    // fire
    bool something_happened = toolbelt_item_begin_alpha_action();
    if (something_happened) send_begin_alpha_action_packet();
}

void left_trigger_up_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
    // fire
    bool something_happened = toolbelt_item_end_alpha_action();
    if (something_happened) send_end_alpha_action_packet();
}

void right_trigger_down_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
    // zoom
    bool something_happened = toolbelt_item_beta_action();
    if (something_happened) send_beta_action_packet();
}

void right_trigger_up_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
}

void reload_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
    // reload
    bool something_happened = toolbelt_item_reload_action();
    if (something_happened) send_reload_action_packet();
}


} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

// use for continuous click-and-hold weapons
void tick_agent_selected_item(int agent_id, ItemID item_id)
{
    if (item_id == NULL_ITEM) return;

    Item::Item* item = Item::get_item(item_id);
    assert(item != NULL);

    if (item->group != IG_MINING_LASER) return;

    item->durability -= 1;
    if (item->durability < 0) item->durability = 0;
    if (item->durability <= 0) Item::destroy_item(item_id);
}

// use for fire_rate trigger events
void trigger_agent_selected_item(int agent_id, ItemID item_id)
{
    // adjust item durability/energy
    // restrict events as needed
    //  -- this need to be integrated with the old hitscan_pick packet handlers

    // later remove this when we have generic action for null item (hand punching or something)
    if (item_id == NULL_ITEM) return;

    Item::Item* item = Item::get_item(item_id);
    assert(item != NULL);

    // 2 modes here:
    // items that are  not click-and-hold will have their new state sent out immediately (here)
    // click-and-hold items will have their state sent out when they are turned off
    // (or when they run out and die). the client will predict state until then
    Item::ItemAttribute* attr = Item::get_item_attributes(item->type);
    assert(attr != NULL);

    // the tick function will handle the state maintenance for click and hold
    if (attr->click_and_hold) return;

    // will need to notify agent of state changes
    Agent_state* a = ServerState::agent_list->get(agent_id);
    
    if (item->group == IG_PLACER)
    {
        int stack_size = item->stack_size;
         // this will automatically destroy item if last stack is used. which also removes it from inventory
        int remaining_stack_size = Item::consume_stack_item(item->id);
        if (remaining_stack_size == 0)
        {
            agent_selected_type[agent_id] = NULL_ITEM_TYPE;
            agent_selected_item[agent_id] = NULL_ITEM;
        }
        else if (stack_size != remaining_stack_size) 
            if (a != NULL) Item::send_item_state(a->client_id, item->id);        
    }
    else if (item->durability != NULL_DURABILITY)   // dont decrement durability for nulls
    {
        item->durability -= 1;
        if (item->durability < 0) item->durability = 0;
        if (item->durability <= 0) Item::destroy_item(item->id);
        else if (a != NULL) Item::send_item_state(a->client_id, item->id);
    }

    agent_fire_on[agent_id] = false;
    agent_fire_tick[agent_id] = 0;
}

void update_toolbelt_items()
{
    assert(agent_selected_type != NULL);
    assert(agent_selected_item != NULL);
    assert(agent_selected_slot != NULL);
    // make sure agent_selected_item is current
    // if any discrepancies exist, send a set_selected_item packet
    for (int agent_id=0; agent_id<AGENT_MAX; agent_id++)
    {
        int slot = agent_selected_slot[agent_id];
        ItemID item_id = Item::get_agent_toolbelt_item(agent_id, slot);
        agent_selected_item[agent_id] = item_id;
        int item_type = Item::get_item_type(item_id);
        if (item_type != agent_selected_type[agent_id])
        {
            agent_selected_type[agent_id] = item_type;
            agent_fire_on[agent_id] = false;
            agent_fire_tick[agent_id] = 0;
            broadcast_agent_set_active_item_packet(agent_id, item_type);
        }
        
    }
}

ItemID get_agent_selected_item(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    return agent_selected_item[agent_id];
}

bool set_agent_toolbelt_slot(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(slot >= 0 && slot < TOOLBELT_MAX_SLOTS && slot != NULL_SLOT);
    agent_selected_slot[agent_id] = slot;
    ItemID item_id = Item::get_agent_toolbelt_item(agent_id, slot);
    if (item_id == agent_selected_item[agent_id]) return false;
    agent_selected_item[agent_id] = item_id;
    return true;
}

} // Toolbelt
#endif
