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
    agent_selected_slot = (int*)calloc(AGENT_MAX, sizeof(int));
    #endif
}

void teardown()
{
    #if DC_CLIENT
    #endif
    #if DC_SERVER
    if (agent_selected_slot != NULL) free(agent_selected_slot);
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

void left_trigger_event()
{
    // fire
}

void right_trigger_event()
{
    // zoom
}

void reload_event()
{
    // reload
}


} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

ItemID get_agent_selected_item(int agent_id)
{
    assert(agent_id >= 0 && agent_id < AGENT_MAX);
    int slot = agent_selected_slot[agent_id];
    if (slot == NULL_SLOT) return NULL_ITEM;
    return Item::get_agent_toolbelt_item(agent_id, slot);
}

void set_agent_toolbelt_slot(int agent_id, int slot)
{
    assert(agent_id >= 0 && agent_id < AGENT_MAX);
    assert(slot >= 0 && slot < TOOLBELT_MAX_SLOTS && slot != NULL_SLOT);
    agent_selected_slot[agent_id] = slot;
}

} // Toolbelt
#endif
