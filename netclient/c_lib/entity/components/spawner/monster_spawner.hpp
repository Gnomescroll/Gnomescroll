#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/spawner.hpp>

namespace Components
{

class MonsterSpawnerComponent: public SpawnerComponent
{
    public:
        ObjectType spawn_type;
        float radius;
        int max_children;
        int children;

        bool full() { return (this->children >= this->max_children); }
        void gain_child(ObjectType type, int id);
        void lose_child(ObjectType type, int id);
        void get_spawn_point(Vec3 position, int spawned_object_height, Vec3* spawn_point);
        
    MonsterSpawnerComponent()
    : SpawnerComponent(COMPONENT_MONSTER_SPAWNER),
    spawn_type(OBJECT_NONE), radius(1.0f), max_children(1), children(0)
    {}
};

}; // Objects
