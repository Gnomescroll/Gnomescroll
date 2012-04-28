#include "CtoS.hpp"

#if DC_CLIENT

namespace Item
{

// dummies
inline void container_action_alpha_CtoS::handle() {}
inline void container_action_beta_CtoS::handle() {}

} // Item
#endif


#if DC_SERVER

#include <state/server_state.hpp>
#include <item/_interface.hpp>
#include <item/server.hpp>

namespace Item
{

inline void container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    // check if failed
    bool succeeded = alpha_action_decision_tree(a->id, client_id, container_id, slot);
    if (!succeeded) send_container_failed_action(client_id, event_id);
}

inline void container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    // check if failed
    bool succeeded = beta_action_decision_tree(a->id, client_id, container_id, slot);
    if (!succeeded) send_container_failed_action(client_id, event_id);
}

} // Item
#endif
