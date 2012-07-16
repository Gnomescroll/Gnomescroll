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

    int x = (int)position.x;
    int y = (int)position.y;

    int sx = randrange(x - this->radius, x + this->radius);
    sx = translate_point(sx);
    spawn_point.x = sx;

    int sy = randrange(y - this->radius, y + this->radius);
    sy = translate_point(sy);
    spawn_point.y = sy;

    int h = (int)ceil(spawned_object_height);
    GS_ASSERT(h >= 1);
    if (h < 1) h = 1;
    spawn_point.z = t_map::get_highest_open_block(sx, sy, h);

    while (object_collides_terrain(spawn_point, spawned_object_height, spawned_object_radius) && spawn_point.z < map_dim.z)
        spawn_point.z += 1;

    return spawn_point;
}

}; // Objects
