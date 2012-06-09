#include "monster_spawner.hpp"

#include <physics/vec3.hpp>
#include <t_map/t_map.hpp>
#include <agent/agent_physics.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

struct Vec3 MonsterSpawnerComponent::get_spawn_point(float spawned_object_height, float spawned_object_radius)
{
    Vec3 spawn_point = vec3_init(0,0,0);

    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return spawn_point;

    Vec3 position = physics->get_position();
    
    float sx,sy;
    sx = position.x + ((randf() * this->radius * 2) - this->radius);
    sx = translate_point(sx);

    sy = position.y + ((randf() * this->radius * 2) - this->radius);
    sy = translate_point(sy);

    spawn_point.x = sx;
    spawn_point.y = sy;
    spawn_point.z = t_map::get_highest_open_block((int)sx, (int)sy, (int)ceil(spawned_object_height));

    while(object_collides_terrain(spawn_point, spawned_object_height, spawned_object_radius) && spawn_point.z < map_dim.z)
        spawn_point.z += 1;

    return spawn_point;
}

Objects::Object* MonsterSpawnerComponent::spawn_child()
{
    GS_ASSERT(this->spawn_type != OBJECT_NONE);    // dont use this method when any component can be spawned
    if (this->spawn_type == OBJECT_NONE) return NULL;
    return this->spawn_child(this->spawn_type);
}

Objects::Object* MonsterSpawnerComponent::spawn_child(ObjectType type)
{
    GS_ASSERT(this->spawn_type == OBJECT_NONE || this->spawn_type == type);    // dont allow this method if type does not match
    if (this->spawn_type != OBJECT_NONE && this->spawn_type != type) return NULL;
    if (this->children >= this->max_children) return NULL;
    Object* object = Objects::create(type);
    GS_ASSERT(object != NULL);
    if (object == NULL) return NULL;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        float height = 1.0f;
        using Components::DimensionComponent;
        DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL) height = dims->get_height();

        float radius = 1.0f;
        using Components::VoxelModelComponent;
        VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
        if (vox != NULL) radius = vox->get_radius();
        
        Vec3 position = this->get_spawn_point(height, radius);
        physics->set_position(position);
    }

    using Components::SpawnChildComponent;
    SpawnChildComponent* child = (SpawnChildComponent*)object->get_component(COMPONENT_SPAWN_CHILD);
    GS_ASSERT(child != NULL);
    if (child == NULL) return object;
    child->assign_parent(this->object->type, this->object->id);
    
    this->children++;

    return object;
}

void MonsterSpawnerComponent::lose_child(ObjectType type, int id)
{
    this->children--;
    GS_ASSERT(this->children >= 0);
    if (this->children < 0) this->children = 0;
}

}; // Objects
