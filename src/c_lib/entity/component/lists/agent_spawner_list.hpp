#pragma once

#include <entity/component/component_list.hpp>

namespace Components
{

const int BASE_SPAWN_ID = 255;

const int MAX_AGENT_SPAWNER_COMPONENTS = 128;
class AgentSpawnerComponentList: public ComponentList<AgentSpawnerComponent, COMPONENT_AGENT_SPAWNER, MAX_AGENT_SPAWNER_COMPONENTS>
{
    public:

        Objects::Object* get_random_spawner();

    AgentSpawnerComponentList()
    {}
};


} // Components

