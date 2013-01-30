#pragma once

namespace Agents
{

void init_attributes();
void teardown_attributes();
void register_attributes();

// API

int get_agent_max_health(AgentID agent_id);
int get_agent_health(AgentID agent_id);

}   // Agents
