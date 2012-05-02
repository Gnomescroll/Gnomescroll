#include "StoC.hpp"

#include <toolbelt/_state.hpp>

namespace Toolbelt
{

#if DC_CLIENT
inline void toolbelt_set_active_item_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    agent_selected_type[agent_id] = item_type;
}

inline void toolbelt_item_begin_alpha_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    agent_fire_on[a->id] = true;
}

inline void toolbelt_item_end_alpha_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    agent_fire_on[a->id] = false;
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
