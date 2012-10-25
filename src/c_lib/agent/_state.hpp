#pragma once

#include <agent/agent_list.hpp>

namespace Agents
{

extern class AgentList* agent_list;

void init_state();
void teardown_state();

}   // Agents
