#pragma once

#include <agent/_state.hpp>

namespace Agents
{

class Agent* create_agent(AgentID id);
class Agent* get_agent(AgentID id);
bool destroy_agent(AgentID id);

#if DC_SERVER
// accesses either main pool or tmp pool
class Agent* get_any_agent(AgentID id);
bool destroy_any_agent(AgentID id);

class Agent* create_temp_agent(AgentID id);
bool destroy_temp_agent(AgentID id);

// transfer temp agent to main pool
class Agent* load_temp_agent(AgentID id);
#endif

void init();
void teardown();


}   // Agents
