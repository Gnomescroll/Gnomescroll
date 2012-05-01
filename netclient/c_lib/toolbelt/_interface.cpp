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

void toolbelt_item_selected_event(int container_id, int slot)
{
    if (container_id == NULL_CONTAINER || container_id != toolbelt_id) return;
    // update selected item
    selected_slot = slot;
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

} // Toolbelt
#endif
