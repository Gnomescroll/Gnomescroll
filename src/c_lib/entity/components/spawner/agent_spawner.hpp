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
        SubscriberList subscribers;    // not for networking, but for who is using this as a spawner
        struct Vec3 get_spawn_point(float spawned_object_height, float spawned_object_radius);
    
    AgentSpawnerComponent()
    : SpawnerComponent(COMPONENT_AGENT_SPAWNER),
    radius(0), subscribers(Objects::AGENT_SPAWNER_INITIAL_CHILDREN_SIZE)
    {}
};

}; // Objects
