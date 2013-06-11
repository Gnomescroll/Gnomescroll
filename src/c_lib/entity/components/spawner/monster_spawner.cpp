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

    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
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
    IF_ASSERT(this->spawn_type == ENTITY_NONE) return NULL;
    if (this->full()) return NULL;
    IF_ASSERT(this->children == NULL) return NULL;
    Entities::Entity* child = Entities::create(this->spawn_type);
    IF_ASSERT(child == NULL) return NULL;

    this->children[this->children_ct++] = child->id;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)child->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        float height = 1.0f;
        using Components::DimensionComponent;
        DimensionComponent* dims = (DimensionComponent*)child->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL) height = dims->get_height();

        float radius = 1.0f;
        using Components::VoxelModelComponent;
        VoxelModelComponent* vox = (VoxelModelComponent*)child->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
        if (vox != NULL) radius = vox->get_radius();

        Vec3 position = this->get_spawn_point(height, radius);
        physics->set_position(position);
    }

    using Components::SpawnChildComponent;
    SpawnChildComponent* spawn = (SpawnChildComponent*)child->get_component(COMPONENT_SPAWN_CHILD);
    GS_ASSERT(spawn != NULL);
    if (spawn != NULL)
        spawn->assign_parent(this->object->type, this->object->id);

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
        SpawnChildComponent* spawn = (SpawnChildComponent*)child->get_component(COMPONENT_SPAWN_CHILD);
        IF_ASSERT(spawn == NULL) continue;
        GS_ASSERT(spawn->parent_type == this->object->type);
        GS_ASSERT(spawn->parent_id == this->object->id);
        spawn->parent_died();
    }
}

}; // Components
