#include "_state.hpp"

#include <common/template/object_list.hpp>
#include <agent/constants.hpp>
#include <agent/agent_list.hpp>

namespace Agents
{

class AgentList* agent_list = NULL;

#if DC_SERVER
class AgentListTemp* agent_list_temp = NULL;
#endif

void init_state()
{
    GS_ASSERT(agent_list == NULL);
    agent_list = new AgentList(MAX_AGENTS);

    #if DC_SERVER
    agent_list_temp = new AgentListTemp(MAX_AGENTS);
    #endif
}

void teardown_state()
{
    if (agent_list != NULL) delete agent_list;

    #if DC_SERVER
    if (agent_list_temp != NULL) delete agent_list_temp;
    #endif
}
    
} // Agents
