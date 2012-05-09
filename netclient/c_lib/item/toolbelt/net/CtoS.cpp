#include "CtoS.hpp"

#include <item/toolbelt/_interface.hpp>
#include <item/toolbelt/_state.hpp>

#if DC_SERVER
#include <item/toolbelt/server.hpp>
#endif

namespace Toolbelt
{

#if DC_CLIENT
inline void toolbelt_set_slot_CtoS::handle() {}
inline void toolbelt_begin_alpha_action_CtoS::handle() {}
inline void toolbelt_end_alpha_action_CtoS::handle() {}
inline void toolbelt_beta_action_CtoS::handle() {}
inline void toolbelt_reload_action_CtoS::handle() {}
#endif

#if DC_SERVER

inline void toolbelt_set_slot_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    bool item_changed = set_agent_toolbelt_slot(a->id, slot);

    ItemID item_id = get_agent_selected_item(a->id);
    if (item_changed) broadcast_agent_set_active_item_packet(a->id, item_id);
}

inline void toolbelt_begin_alpha_action_CtoS::handle() 
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    
    agent_fire_on[a->id] = true;

    ItemID item_id = get_agent_selected_item(a->id);
    broadcast_agent_toolbelt_begin_alpha_action_packet(a->id, item_id);
}

inline void toolbelt_end_alpha_action_CtoS::handle() 
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    agent_fire_on[a->id] = false;
    agent_fire_tick[a->id] = 0;
    broadcast_agent_toolbelt_end_alpha_action_packet(a->id);
}

inline void toolbelt_beta_action_CtoS::handle() 
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    ItemID item_id = get_agent_selected_item(a->id);
    if (item_id == NULL_ITEM) return;
    broadcast_agent_toolbelt_beta_action_packet(a->id, item_id);
}

inline void toolbelt_reload_action_CtoS::handle() 
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    ItemID item_id = get_agent_selected_item(a->id);
    if (item_id == NULL_ITEM) return;
    broadcast_agent_toolbelt_item_reload_packet(a->id, item_id);
}

#endif

} // Toolbelt
