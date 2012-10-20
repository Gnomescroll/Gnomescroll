#pragma once

#include <agent/agent_list.hpp>

namespace Agents
{

extern class AgentList* agent_list;

#if DC_SERVER
extern class AgentListTemp* agent_list_temp;
#endif

void init_state();
void teardown_state();

}   // Agents
