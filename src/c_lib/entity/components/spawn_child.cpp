#include "spawn_child.hpp"

namespace Components
{

void SpawnChildComponent::notify_parent_of_death()
{
    if (this->parent_id == NULL_ENTITY) return;

    Entities::Entity* obj = Entities::get(this->parent_type, this->parent_id);
    if (obj == NULL) return;

    using Components::SpawnerComponent;
    MonsterSpawnerComponent* spawner = (MonsterSpawnerComponent*)obj->get_component(COMPONENT_MONSTER_SPAWNER);
    if (spawner == NULL) return;
    spawner->lose_child(this->object->type, this->object->id);
}

void SpawnChildComponent::parent_died()
{
    this->parent_id = NULL_ENTITY;
}

}; // Components
