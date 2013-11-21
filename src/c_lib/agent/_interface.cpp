/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

namespace Agents
{

class Agent* create_agent(AgentID id)
{
    class Agent* agent = agent_list->create(id);
    IF_ASSERT(agent == NULL) return NULL;
    agent->init_vox();
    return agent;
}

class Agent* get_agent(AgentID id)
{
    if (id == NULL_AGENT) return NULL;
    return agent_list->get(id);
}

bool destroy_agent(AgentID id)
{
    bool destroyed = agent_list->destroy(id);
    if (!destroyed) printf("Was not destroyed (didnt exist?)\n");
    if (!destroyed) return false;

    Toolbelt::remove_agent(id);

    #if DC_SERVER
    agent_destroy_StoC msg;
    msg.id = id;
    msg.broadcast();
    #endif

    return true;
}

void init()
{
    init_state();
}

void teardown()
{
    teardown_state();
}

}   // Agents
