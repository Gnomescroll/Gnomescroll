#pragma once

#include <entity/constants.hpp>
#include <entity/components/spawner.hpp>

namespace Components
{

class MonsterSpawnerComponent: public SpawnerComponent
{
    private:
        int max_children;
        int* children;

    public:
        EntityType spawn_type;
        float radius;
        int children_ct;

    bool full()
    {
        return (this->children_ct >= this->max_children);
    }

    Entities::Entity* spawn_child();
    void lose_child(EntityType type, int id);
    struct Vec3 get_spawn_point(float spawned_object_height, float spawned_object_radius);

    void notify_children_of_death();

    void set_max_children(int max_children)
    {
        if (this->max_children == max_children) return;

        if (this->children != NULL)
        {
            int* tmp = (int*)realloc(this->children, max_children * sizeof(int));
            GS_ASSERT(tmp != NULL);
            if (tmp != NULL)
                this->children = tmp;
            else
                max_children = this->max_children;  // cancel growth
        }
        else
            this->children = (int*)malloc(max_children * sizeof(int));

        for (int i=0; i<max_children; i++)
            this->children[i] = NULL_ENTITY_ID;

        this->max_children = max_children;
    }

    MonsterSpawnerComponent() :
        SpawnerComponent(COMPONENT_MONSTER_SPAWNER),
        max_children(0), children(NULL), spawn_type(OBJECT_NONE),
        radius(1.0f), children_ct(0)
    {}

    ~MonsterSpawnerComponent()
    {
        free(this->children);
    }
};

}; // Components
