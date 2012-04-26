#include "owner.hpp"

#include <agent/constants.hpp>
#include <agent/agent.hpp>
#include <agent/agent_list.hpp>
#include <state/server_state.hpp>

namespace Components
{

#if DC_SERVER
void OwnerComponent::revoke()
{
    if (this->owner == NO_AGENT) return;
    Agent_state* a = ServerState::agent_list->get(this->owner);
    if (a != NULL) a->status.lose_item(this->object->type);
    this->owner = NO_AGENT;
}
#endif

} // Objects
