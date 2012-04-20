#include "owner.hpp"

#include <c_lib/agent/constants.hpp>
#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/agent_list.hpp>
#include <c_lib/state/server_state.hpp>

namespace Components
{

#if DC_SERVER
void OwnerComponent::revoke()
{
    if (this->owner == NO_AGENT) return;
    Agent_state* a = ServerState::agent_list->get(this->owner);
    if (a != NULL) a->status.lose_item(this->object->type);
}
#endif

} // Objects
