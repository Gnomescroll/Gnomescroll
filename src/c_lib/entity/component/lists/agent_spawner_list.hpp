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

    const int get_spawner_for_user(UserID user_id)
    {
        for (int i=0; i<this->max; i++)
            if (this->components[i] != NULL)
                for (size_t j=0; j<this->components[i]->users.count; j++)
                    if (this->components[i]->users.subscribers[j] == user_id)
                        return this->components[i]->object->id;
        return BASE_SPAWN_ID;
    }

    AgentSpawnerComponentList()
    {}
};


} // Components

