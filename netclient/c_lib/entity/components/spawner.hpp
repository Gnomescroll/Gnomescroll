#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class SpawnerComponent: public Component
{
    public:
        virtual void get_spawn_point(int spawned_object_height, Vec3* spawn_point) = 0;
    
    explicit SpawnerComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_SPAWNER)
    {}

    virtual ~SpawnerComponent() {}
};

}; // Objects
