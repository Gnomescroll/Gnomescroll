#pragma once

#include <entity/constants.hpp>
#include <entity/components/spawner.hpp>

namespace Components
{

class AgentSpawnerComponent: public SpawnerComponent
{
    public:
        float radius;
        struct Vec3 get_spawn_point(float spawned_object_height, float spawned_object_radius);
    
    AgentSpawnerComponent()
    : SpawnerComponent(COMPONENT_AGENT_SPAWNER),
    radius(1.0f)
    {}
};

}; // Objects
