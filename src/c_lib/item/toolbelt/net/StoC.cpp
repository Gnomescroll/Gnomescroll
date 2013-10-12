#include "StoC.hpp"

#include <item/toolbelt/_state.hpp>
#if DC_CLIENT
# include <item/toolbelt/client.hpp>
#endif

namespace Toolbelt
{

#if DC_CLIENT
inline void toolbelt_set_active_item_StoC::handle()
{
    AgentID agent_id = (AgentID)this->agent_id;
    ItemType item_type = (ItemType)this->item_type;
    IF_ASSERT(!isValid(agent_id)) return;
    IF_ASSERT(agent_selected_type == NULL) return;
    if (item_type == agent_selected_type[this->agent_id]) return;
    set_agent_selected_item_type(agent_id, item_type);
}

inline void toolbelt_item_begin_alpha_action_StoC::handle()
{
    if (agent_id == ClientState::player_agent.agent_id) return;    // ignore ourself
    turn_fire_on(AgentID(this->agent_id));

    // TODO --
        // This is needed for indicating other agents' went on
        // BUT we only want the server's command when it is forcing us to begin,
        // i.e. we did not initiate it
        // So we will need 2 packets (don't really need the force command one right yet)
}

inline void toolbelt_item_end_alpha_action_StoC::handle()
{
    turn_fire_off((AgentID)this->agent_id);
}

inline void toolbelt_item_alpha_action_StoC::handle()
{
    if (agent_id == ClientState::player_agent.agent_id) return;
    trigger_item((AgentID)this->agent_id, (ItemType)this->item_type);
}

inline void toolbelt_item_beta_action_StoC::handle()
{
    if (agent_id == ClientState::player_agent.agent_id) return;
    // play sound/anim
    trigger_item_beta((AgentID)this->agent_id, (ItemType)this->item_type);
}
#endif

#if DC_SERVER
inline void toolbelt_set_active_item_StoC::handle() {}
inline void toolbelt_item_begin_alpha_action_StoC::handle() {}
inline void toolbelt_item_end_alpha_action_StoC::handle() {}
inline void toolbelt_item_alpha_action_StoC::handle() {}
inline void toolbelt_item_beta_action_StoC::handle() {}
#endif

} // Toolbelt
