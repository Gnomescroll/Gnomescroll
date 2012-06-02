#pragma once

#include <entity/constants.hpp>
#include <entity/components/spawner.hpp>

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
        Objects::Object* spawn_child();
        Objects::Object* spawn_child(ObjectType type);
        void lose_child(ObjectType type, int id);
        struct Vec3 get_spawn_point(float spawned_object_height, float spawned_object_radius);
        
    MonsterSpawnerComponent()
    : SpawnerComponent(COMPONENT_MONSTER_SPAWNER),
    spawn_type(OBJECT_NONE), radius(1.0f), max_children(1), children(0)
    {}
};

}; // Objects
