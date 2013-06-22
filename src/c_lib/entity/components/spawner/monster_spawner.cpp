#include "monster_spawner.hpp"

#include <physics/vec3.hpp>
#include <t_map/t_map.hpp>
#include <agent/agent_physics.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

struct Vec3 MonsterSpawnerComponent::get_spawn_point(
    float spawned_object_height, float spawned_object_radius)
{
    Vec3 spawn_point = vec3_init(0);

    auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
    if (physics == NULL) return spawn_point;

    Vec3 position = physics->get_position();
    float sx = position.x + ((randf() * this->radius * 2) - this->radius);
    sx = translate_point(sx);
    float sy = position.y + ((randf() * this->radius * 2) - this->radius);
    sy = translate_point(sy);

    spawn_point.x = sx;
    spawn_point.y = sy;
    spawn_point.z = t_map::get_nearest_surface_block(sx, sy, ceilf(spawned_object_height));
    while (object_collides_terrain(spawn_point, spawned_object_height, spawned_object_radius) &&
           spawn_point.z < map_dim.z)
    {
        spawn_point.z += 1;
    }
    return spawn_point;
}

class Entities::Entity* MonsterSpawnerComponent::spawn_child()
{
    IF_ASSERT(this->spawn_type == NULL_ENTITY_TYPE) return NULL;
    if (this->full()) return NULL;
    IF_ASSERT(this->children == NULL) return NULL;
    Entities::Entity* child = Entities::create(this->spawn_type);
    IF_ASSERT(child == NULL) return NULL;

    this->children[this->children_ct++] = child->id;

    auto physics = GET_COMPONENT_INTERFACE(Physics, child);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        float height = 1.0f;
        auto dims = GET_COMPONENT_INTERFACE(Dimension, child);
        if (dims != NULL) height = dims->get_height();

        float radius = 1.0f;
        auto vox = GET_COMPONENT_INTERFACE(VoxelModel, child);
        if (vox != NULL) radius = vox->get_radius();

        Vec3 position = this->get_spawn_point(height, radius);
        physics->set_position(position);
    }

    auto spawn = GET_COMPONENT(SpawnChild, child);
    GS_ASSERT(spawn != NULL);
    if (spawn != NULL)
        spawn->assign_parent(this->entity->type, this->entity->id);

    return child;
}

void MonsterSpawnerComponent::lose_child(EntityType type, int id)
{
    for (int i=0; i<this->children_ct; i++)
        if (this->children[i] == id)
            this->children[i] = this->children[--this->children_ct];  // swap
}

void MonsterSpawnerComponent::notify_children_of_death()
{
    for (int i=0; i<this->children_ct; i++)
    {
        class Entities::Entity* child = Entities::get(this->spawn_type, this->children[i]);
        IF_ASSERT(child == NULL) continue;
        auto spawn = GET_COMPONENT(SpawnChild, child);
        IF_ASSERT(spawn == NULL) continue;
        GS_ASSERT(spawn->parent_type == this->entity->type);
        GS_ASSERT(spawn->parent_id == this->entity->id);
        spawn->parent_died();
    }
}

}; // Components
