#include "owner.hpp"

#include <agent/constants.hpp>
#include <agent/agent.hpp>
#include <agent/agent_list.hpp>
#include <state/state.hpp>

namespace Components
{

#if DC_SERVER
void OwnerComponent::revoke()
{
    if (this->owner == NULL_AGENT) return;
    this->owner = NULL_AGENT;
}
#endif

} // Entities
