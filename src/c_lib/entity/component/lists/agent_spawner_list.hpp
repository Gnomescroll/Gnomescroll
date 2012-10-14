#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <entity/component/component_list.hpp>

namespace Components
{

const int MAX_AGENT_SPAWNER_COMPONENTS = MAX_SPAWNERS + 1;  // +1 for the base
class AgentSpawnerComponentList: public ComponentList<AgentSpawnerComponent, COMPONENT_AGENT_SPAWNER, MAX_AGENT_SPAWNER_COMPONENTS>
{
    public:

    AgentSpawnerComponentList()
    {}
};


} // Components

