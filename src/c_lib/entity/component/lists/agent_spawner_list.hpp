#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <entity/component/component_list.hpp>

namespace Components
{

const int MAX_AGENT_SPAWNER_COMPONENTS = 1024;
class AgentSpawnerComponentList: public ComponentList<AgentSpawnerComponent, COMPONENT_AGENT_SPAWNER, MAX_AGENT_SPAWNER_COMPONENTS>
{
    public:

    AgentSpawnerComponentList()
    {}
};


} // Components

