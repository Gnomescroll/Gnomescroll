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

namespace Item
{

inline void container_action_alpha_CtoS::handle()
{
    // check if failed
}

inline void container_action_beta_CtoS::handle()
{
    // check if failed
}

} // Item
#endif
