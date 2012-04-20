#pragma once

#include <c_lib/entity/component/component_list.hpp>

namespace Components
{

const int MAX_AGENT_SPAWNER_COMPONENTS = 128;
class AgentSpawnerComponentList: public ComponentList<AgentSpawnerComponent, COMPONENT_AGENT_SPAWNER, MAX_AGENT_SPAWNER_COMPONENTS>
{
    public:

        int max_per_team;
    
        bool team_spawner_available(int team);
        int get_random_spawner(int team);
        int get_numbered_team_spawner(int team, int id);
        AgentSpawnerComponent* get_by_team_index(int team, int team_index);
        bool spawner_exists(int team, int team_index);
        void assign_team_index(Object* spawner);

    AgentSpawnerComponentList()
    : max_per_team(9)
    {}
};


} // Components

