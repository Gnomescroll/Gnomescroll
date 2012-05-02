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
            trigger_agent_selected_item(agent_selected_item[i]);
            #endif
        }
        #if DC_CLIENT
        if (local_agent_id == i) tick_local_agent_selected_item_type(agent_selected_type[i]);
        else tick_agent_selected_item_type(i, agent_selected_type[i]);
        #endif
        #if DC_SERVER
        tick_agent_selected_item(agent_selected_item[i]);
        #endif
        agent_fire_tick[i]++;
    }
}

void remove_agent(int agent_id)
{
    assert(agent_id >= 0 && agent_id < AGENT_MAX);

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
    Item::ItemContainer* toolbelt = Item::get_container(toolbelt_id);
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
    a->event.fired_mining_laser();
}

// tick for the local agent
void tick_local_agent_selected_item_type(int item_type)
{
    ClientState::playerAgent_state.action.tick_mining_laser();
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
    if (container_id == NULL_CONTAINER || container_id != toolbelt_id) return;
    // update selected item
    selected_slot = slot;
    send_set_slot_packet(slot);
    update_selected_item_type();
}

void left_trigger_down_event()
{
    // fire
    bool something_happened = toolbelt_item_begin_alpha_action();
    if (something_happened) send_begin_alpha_action_packet();
}

void left_trigger_up_event()
{
    // fire
    bool something_happened = toolbelt_item_end_alpha_action();
    if (something_happened) send_end_alpha_action_packet();
}

void right_trigger_down_event()
{
    // zoom
    bool something_happened = toolbelt_item_beta_action();
    if (something_happened) send_beta_action_packet();
}

void right_trigger_up_event()
{
}

void reload_event()
{
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
void tick_agent_selected_item(ItemID item_id)
{
    
}

// use for fire_rate trigger events
void trigger_agent_selected_item(ItemID item_id)
{
    //printf("trigger item %d\n", item_id);
    
    // adjust item durability/energy
    // restrict events as needed
    //  -- this need to be integrated with the old hitscan_pick packet handlers
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
            broadcast_agent_set_active_item_packet(agent_id, item_type);
        }
        
    }
}

ItemID get_agent_selected_item(int agent_id)
{
    assert(agent_id >= 0 && agent_id < AGENT_MAX);
    return agent_selected_item[agent_id];
}

bool set_agent_toolbelt_slot(int agent_id, int slot)
{
    assert(agent_id >= 0 && agent_id < AGENT_MAX);
    assert(slot >= 0 && slot < TOOLBELT_MAX_SLOTS && slot != NULL_SLOT);
    agent_selected_slot[agent_id] = slot;
    ItemID item_id = Item::get_agent_toolbelt_item(agent_id, slot);
    if (item_id == agent_selected_item[agent_id]) return false;
    agent_selected_item[agent_id] = item_id;
    return true;
}

} // Toolbelt
#endif
