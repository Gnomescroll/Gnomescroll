#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class SpawnChildComponent: public Component
{
    public:
        EntityType parent_type;
        EntityID parent_id;

        void assign_parent(EntityType type, EntityID id)
        {
            this->parent_type = type;
            this->parent_id = id;
        }
        void notify_parent_of_death();
        void parent_died();

    ~SpawnChildComponent() {}

    SpawnChildComponent() :
        Component(COMPONENT_SPAWN_CHILD, COMPONENT_INTERFACE_SPAWN_CHILD),
        parent_type(NULL_ENTITY_TYPE), parent_id(NULL_ENTITY)
    {}
};

}; // Components
