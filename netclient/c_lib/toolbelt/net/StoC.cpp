#include "StoC.hpp"

namespace Toolbelt
{

#if DC_CLIENT
inline void toolbelt_set_active_item_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
}

inline void toolbelt_item_begin_alpha_action_StoC::handle() 
{
    printf("begin\n");
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
}

inline void toolbelt_item_end_alpha_action_StoC::handle() 
{
    printf("end\n");
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
}

inline void toolbelt_item_beta_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
}

inline void toolbelt_item_reload_action_StoC::handle() 
{
    if (agent_id == ClientState::playerAgent_state.agent_id) return;
}
#endif

#if DC_SERVER
inline void toolbelt_set_active_item_StoC::handle() {}
inline void toolbelt_item_begin_alpha_action_StoC::handle() {}
inline void toolbelt_item_end_alpha_action_StoC::handle() {}
inline void toolbelt_item_beta_action_StoC::handle() {}
inline void toolbelt_item_reload_action_StoC::handle() {}
#endif

} // Toolbelt
