#pragma once

#include <entity/constants.hpp>
#include <entity/components/spawner.hpp>

namespace Components
{

class MonsterSpawnerComponent: public SpawnerComponent
{
    private:
        int max_children;
        EntityID* children;

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

    inline int get_max_children() const
    {
        return this->max_children;
    }

    void set_max_children(int max_children)
    {
        if (this->max_children == max_children) return;
        IF_ASSERT(this->children != NULL) return;
        this->children = (EntityID*)malloc(max_children * sizeof(*this->children));
        for (int i=0; i<max_children; i++)
            this->children[i] = NULL_ENTITY;
        this->max_children = max_children;
    }

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(MonsterSpawnerComponent);
        COPY(radius);
        COPY(spawn_type);
        this->set_max_children(_component->get_max_children());
    }

    MonsterSpawnerComponent() :
        SpawnerComponent(COMPONENT_MonsterSpawner),
        max_children(0), children(NULL), spawn_type(NULL_ENTITY_TYPE),
        radius(1.0f), children_ct(0)
    {}

    virtual ~MonsterSpawnerComponent()
    {
        free(this->children);
    }
};

}; // Components
