/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <agent/_state.hpp>

namespace Agents
{

class Agent* create_agent(AgentID id);
class Agent* get_agent(AgentID id);
bool destroy_agent(AgentID id);

void init();
void teardown();

}   // Agents
