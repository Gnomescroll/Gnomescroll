#include "_state.hpp"

#include <common/template/object_list.hpp>
#include <agent/constants.hpp>
#include <agent/agent_list.hpp>

namespace Agents
{

class AgentList* agent_list = NULL;

void init_state()
{
    GS_ASSERT(agent_list == NULL);
    agent_list = new AgentList(MAX_AGENTS);
}

void teardown_state()
{
    if (agent_list != NULL) delete agent_list;
}
    
} // Agents
