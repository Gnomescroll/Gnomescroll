/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <agent/agent_list.hpp>

namespace Agents
{

extern class AgentList* agent_list;

void init_state();
void teardown_state();

}   // Agents
