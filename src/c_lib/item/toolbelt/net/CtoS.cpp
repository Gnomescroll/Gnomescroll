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
#endif

#if DC_SERVER

inline void toolbelt_set_slot_CtoS::handle()
{
    class Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    bool item_changed = set_agent_toolbelt_slot(a->id, slot);
    if (!item_changed) return;

    ItemID item_id = get_agent_selected_item(a->id);
    int item_type = Item::get_item_type(item_id);
    broadcast_agent_set_active_item_packet(a->id, item_type);
}

inline void toolbelt_begin_alpha_action_CtoS::handle() 
{
    Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    
    turn_fire_on(a->id);
}

inline void toolbelt_end_alpha_action_CtoS::handle() 
{
    class Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    turn_fire_off(a->id);
}

inline void toolbelt_beta_action_CtoS::handle() 
{
    Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    ItemID item_id = get_agent_selected_item(a->id);
    int item_type = Item::get_item_type(item_id);
    trigger_item_beta(a->id, item_id, item_type);
    broadcast_agent_toolbelt_beta_action_packet(a->id, item_type);
}

#endif

} // Toolbelt
