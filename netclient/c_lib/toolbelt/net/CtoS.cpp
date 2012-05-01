#include "CtoS.hpp"

#include <toolbelt/_interface.hpp>

namespace Toolbelt
{

#if DC_CLIENT
inline void toolbelt_set_slot_CtoS::handle() {}
inline void toolbelt_alpha_action_CtoS::handle() {}
inline void toolbelt_beta_action_CtoS::handle() {}
inline void toolbelt_reload_action_CtoS::handle() {}
#endif

#if DC_SERVER

inline void toolbelt_set_slot_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    set_agent_toolbelt_slot(a->id, slot);
}

inline void toolbelt_alpha_action_CtoS::handle() 
{

}

inline void toolbelt_beta_action_CtoS::handle() 
{

}

inline void toolbelt_reload_action_CtoS::handle() 
{

}


#endif

} // Toolbelt
