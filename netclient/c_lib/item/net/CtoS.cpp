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
    // check if failed
    bool succeeded = alpha_action_decision_tree(client_id, inventory_id, slot);
    if (!succeeded) send_container_failed_action(client_id, event_id);
}

inline void container_action_beta_CtoS::handle()
{
    // check if failed
    bool succeeded = beta_action_decision_tree(client_id, inventory_id, slot);
    if (!succeeded) send_container_failed_action(client_id, event_id);
}

} // Item
#endif
