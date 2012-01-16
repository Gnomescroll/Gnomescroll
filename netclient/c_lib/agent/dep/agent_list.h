#ifndef objects_agent_list_h
#define objects_agent_list_h

#include "agent.h"


struct Agent_state* get_agent(int id);
void destroy_agent(int id);
void agent_tick();

#endif
