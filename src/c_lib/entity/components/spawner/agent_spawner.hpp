#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <entity/constants.hpp>
#include <entity/components/spawner.hpp>

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
        class Agents::Agent* agent = Agents::get_agent(agent_id);
        IF_ASSERT(agent == NULL) return;
        if (this->agents.full()) return;
        this->agents.add(agent->id);
        this->add_user(agent->user_id);
    }

    void remove_all(AgentID agent_id)
    {
        this->remove(agent_id);
        class Agents::Agent* agent = Agents::get_agent(agent_id);
        IF_ASSERT(agent == NULL) return;
        this->users.remove(agent->user_id);
    }

    void remove(AgentID agent_id)
    {
        this->agents.remove(agent_id);
    }

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(AgentSpawnerComponent);
        COPY(radius);
    }

    virtual void on_destroy();

    virtual ~AgentSpawnerComponent() {}

    AgentSpawnerComponent() :
        SpawnerComponent(COMPONENT_AgentSpawner),
        radius(0),
        agents(AGENT_SPAWNER_INITIAL_CHILDREN_SIZE, MAX_AGENTS),
        users(AGENT_SPAWNER_INITIAL_USERS_SIZE, MAX_USERS_PER_AGENT_SPAWNER)
    {}
};

}; // Components
