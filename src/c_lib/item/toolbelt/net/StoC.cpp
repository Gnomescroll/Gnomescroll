#include "StoC.hpp"

#include <item/toolbelt/_state.hpp>

namespace Toolbelt
{

#if DC_CLIENT
inline void toolbelt_set_active_item_StoC::handle() 
{
    //if (agent_id == ClientState::playerAgent_state.agent_id) return;
    int old_type = agent_selected_type[agent_id];
    agent_selected_type[agent_id] = item_type;
    if (old_type != item_type)
    {   // the selected item was force-changed by the server
        agent_fire_on[agent_id] = false;
        agent_fire_tick[agent_id] = 0;
    }
}

inline void toolbelt_item_begin_alpha_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    agent_fire_on[a->id] = true;
    agent_fire_tick[a->id] = 0;
}

inline void toolbelt_item_end_alpha_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    agent_fire_on[a->id] = false;
    agent_fire_tick[a->id] = 0;
}

inline void toolbelt_item_beta_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    // play sound/anim
}

inline void toolbelt_item_reload_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    // play sound/anim
}

inline void toolbelt_item_fire_action_StoC::handle()
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    // play sound/anim
}
#endif

#if DC_SERVER
inline void toolbelt_set_active_item_StoC::handle() {}
inline void toolbelt_item_begin_alpha_action_StoC::handle() {}
inline void toolbelt_item_end_alpha_action_StoC::handle() {}
inline void toolbelt_item_beta_action_StoC::handle() {}
inline void toolbelt_item_reload_action_StoC::handle() {}
inline void toolbelt_item_fire_action_StoC::handle() {}
#endif

} // Toolbelt
