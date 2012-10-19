#include "_interface.hpp"

namespace Agents
{

class Agent* create_agent(AgentID id)
{
    class Agent* agent = agent_list->create(id);
    GS_ASSERT(agent != NULL);
    if (agent == NULL) return NULL;
    agent->init_vox();
    return agent;
}

class Agent* get_agent(AgentID id)
{
    return agent_list->get(id);
}

bool destroy_agent(AgentID id)
{
    bool destroyed = agent_list->destroy(id);
    if (!destroyed) return false;
    
    Toolbelt::remove_agent(id);

    #if DC_SERVER
    agent_destroy_StoC msg;
    msg.id = id;
    msg.broadcast();
    #endif

    return true;
}

#if DC_SERVER
// returns agent from main pool or tmp pool
class Agent* get_any_agent(AgentID id)
{
    class Agent* agent = agent_list->get(id);
    if (agent == NULL) agent = agent_list_temp->get(id);
    return agent;
}

bool destroy_any_agent(AgentID id)
{
    if (destroy_agent(id)) return true;
    return destroy_temp_agent(id);
}

class Agent* create_temp_agent(AgentID id)
{
    return agent_list_temp->create(id);
}

bool destroy_temp_agent(AgentID id)
{
    return agent_list_temp->destroy(id);
}

class Agent* load_temp_agent(AgentID id)
{
    class Agent* agent = agent_list_temp->get(id);
    GS_ASSERT(agent != NULL);
    if (agent == NULL) return NULL;
    class Agent* loaded_agent = agent_list->load(agent);
    agent_list_temp->destroy(id);
    return loaded_agent;
}
#endif

void init()
{
    init_state();
}

void teardown()
{
    teardown_state();
}

}   // Agents
