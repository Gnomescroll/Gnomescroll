#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class SpawnerComponent: public Component
{
    public:
    virtual struct Vec3 get_spawn_point(float spawned_object_height, float spawned_object_radius) = 0;

    explicit SpawnerComponent(ComponentType type) :
        Component(type, COMPONENT_INTERFACE_Spawner)
    {}

    virtual ~SpawnerComponent() {}
};

}; // Components
