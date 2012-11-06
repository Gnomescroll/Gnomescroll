#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <entity/constants.hpp>
#include <entity/components/spawner.hpp>
#include <entity/objects/fabs/constants.hpp>

namespace Components
{

class AgentSpawnerComponent: public SpawnerComponent
{
    public:
        unsigned int radius;
        SubscriberList<AgentID> agents;
        SubscriberList<UserID> users;
        
        struct Vec3 get_spawn_point(float spawned_object_height, float spawned_object_radius);

        void add_user(UserID user_id)
        {
            this->users.add(user_id);
        }

        void add_agent(AgentID agent_id)
        {
            class Agent* agent = Agents::get_agent(agent_id);
            GS_ASSERT(agent != NULL);
            if (agent == NULL) return;
            if (this->agents.full()) return;
            this->agents.add(agent->id);
            this->add_user(agent->user_id);
        }

        void remove_all(AgentID agent_id)
        {
            this->remove(agent_id);
            class Agent* agent = Agents::get_agent(agent_id);
            GS_ASSERT(agent != NULL);
            if (agent == NULL) return;
            this->users.remove(agent->user_id);
        }

        void remove(AgentID agent_id)
        {
            this->agents.remove(agent_id);
        }
    
    AgentSpawnerComponent() : SpawnerComponent(COMPONENT_AGENT_SPAWNER),
        radius(0),
        agents(Entities::AGENT_SPAWNER_INITIAL_CHILDREN_SIZE, MAX_AGENTS),
        users(Entities::AGENT_SPAWNER_INITIAL_USERS_SIZE, Entities::MAX_USERS_PER_AGENT_SPAWNER)
    {}
};

}; // Components
