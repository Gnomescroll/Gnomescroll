#include "monster_spawner.hpp"

#include <physics/vec3.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

void MonsterSpawnerComponent::get_spawn_point(int spawned_object_height, Vec3* spawn_point)
{
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL)
    {
        spawn_point->x = 0;
        spawn_point->y = 0;
        spawn_point->z = 0;
        return;
    }

    Vec3 position = physics->get_position();
    
    float sx,sy;
    sx = position.x + ((randf() * this->radius * 2) - this->radius);
    sx = (sx > map_dim.x - 1) ? map_dim.x - 1 : sx;
    sx = (sx < 0) ? 0 : sx;

    sy = position.y + ((randf() * this->radius * 2) - this->radius);
    sy = (sy < map_dim.y - 1) ? sy : map_dim.y - 1;
    sy = (sy < 0) ? 0 : sy;

    int h = (int)ceil(spawned_object_height);
    spawn_point->x = sx;
    spawn_point->y = sy;
    spawn_point->z = t_map::get_highest_open_block((int)sx, (int)sy, h);
}

Objects::Object* MonsterSpawnerComponent::spawn_child()
{
    assert(this->spawn_type != OBJECT_NONE);    // dont use this method when any component can be spawned
    return this->spawn_child(this->spawn_type);
}

Objects::Object* MonsterSpawnerComponent::spawn_child(ObjectType type)
{
    assert(this->spawn_type == OBJECT_NONE || this->spawn_type == type);    // dont allow this method if type does not match
    if (this->children >= this->max_children) return NULL;
    Object* object = Objects::create(type);
    if (object == NULL) return NULL;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        int height = 1;
        using Components::DimensionComponent;
        DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL) height = dims->get_integer_height();
        
        Vec3 position;
        this->get_spawn_point(height, &position);
        physics->set_position(position);
    }

    using Components::SpawnChildComponent;
    SpawnChildComponent* child = (SpawnChildComponent*)object->get_component(COMPONENT_SPAWN_CHILD);
    if (child == NULL) return object;
    child->assign_parent(this->object->type, this->object->id);
    
    this->children++;

    return object;
}

void MonsterSpawnerComponent::lose_child(ObjectType type, int id)
{
    this->children--;
    assert(this->children >= 0);
}

}; // Objects
