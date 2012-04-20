#include "monster_spawner.hpp"

#include <c_lib/physics/vec3.hpp>
#include <c_lib/entity/components/physics.hpp>

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

void MonsterSpawnerComponent::gain_child(ObjectType type, int id)
{
    this->children++;
}

void MonsterSpawnerComponent::lose_child(ObjectType type, int id)
{
    this->children--;
}

}; // Objects
