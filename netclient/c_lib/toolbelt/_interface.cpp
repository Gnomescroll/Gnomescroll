#include "_interface.hpp"

// Common
namespace Toolbelt
{

void init()
{
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
    #if DC_CLIENT
    #endif
    #if DC_SERVER
    if (agent_selected_slot != NULL) free(agent_selected_slot);
    if (agent_selected_item != NULL) free(agent_selected_item);
    #endif
}


} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

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
}

void left_trigger_down_event()
{
    // fire
    toolbelt_item_begin_alpha_action();
    send_begin_alpha_action_packet();
}

void left_trigger_up_event()
{
    // fire
    toolbelt_item_end_alpha_action();
    send_end_alpha_action_packet();
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

void update_toolbelt_items()
{
    assert(agent_selected_item != NULL);
    assert(agent_selected_slot != NULL);
    // make sure agent_selected_item is current
    // if any discrepancies exist, send a set_selected_item packet
    for (int agent_id=0; agent_id<AGENT_MAX; agent_id++)
    {
        int slot = agent_selected_slot[agent_id];
        ItemID item_id = Item::get_agent_toolbelt_item(agent_id, slot);
        if (item_id != agent_selected_item[agent_id])
        {
            agent_selected_item[agent_id] = item_id;
            broadcast_agent_set_active_item_packet(agent_id, item_id);
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
