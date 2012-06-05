#include "agent_spawner.hpp"

#include <physics/vec3.hpp>
#include <t_map/t_map.hpp>
#include <entity/components/physics.hpp>
#include <agent/agent_physics.hpp>

namespace Components
{

struct Vec3 AgentSpawnerComponent::get_spawn_point(float spawned_object_height, float spawned_object_radius)
{
    Vec3 spawn_point = vec3_init(0,0,0);

    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return spawn_point;

    Vec3 position = physics->get_position();

    int x,y;
    x = (int)position.x;
    y = (int)position.y;

    int sx,sy;

    sx = randrange(x - this->radius, x + this->radius);
    sx = translate_point(sx);
    spawn_point.x = sx;

    sy = randrange(y - this->radius, y + this->radius);
    sy = translate_point(sy);
    spawn_point.y = sy;

    spawn_point.z = _get_highest_open_block(sx, sy, (int)ceil(spawned_object_height));

    while(object_collides_terrain(spawn_point, spawned_object_height, spawned_object_radius) && spawn_point.z < map_dim.z)
        spawn_point.z += 1;

    return spawn_point;
}

}; // Objects
