#include "spawn_child.hpp"

namespace Components
{

void SpawnChildComponent::notify_parent_of_death()
{
    Objects::Object* obj = Objects::get(this->parent_type, this->parent_id);
    if (obj == NULL) return;

    using Components::SpawnerComponent;
    MonsterSpawnerComponent* spawner = (MonsterSpawnerComponent*)obj->get_component(COMPONENT_MONSTER_SPAWNER);
    assert(spawner != NULL);
    spawner->lose_child(this->object->type, this->object->id);
}

}; // Objects
