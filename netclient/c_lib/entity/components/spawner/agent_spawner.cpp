#include "agent_spawner.hpp"

#include <physics/vec3.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

void AgentSpawnerComponent::get_spawn_point(int spawned_object_height, Vec3* spawn_point)
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
    
    int x,y;
    x = (int)position.x;
    y = (int)position.y;

    int sx,sy;

    sx = randrange(x - this->radius, x + this->radius);
    sx = (sx < map_dim.x) ? sx : map_dim.x;
    sx = (spawn_point->x < 0) ? 0 : spawn_point->x;
    spawn_point->x = sx;

    sy = randrange(y - this->radius, y + this->radius);
    sy = (sy < map_dim.y) ? sy : map_dim.y;
    sy = (spawn_point->y < 0) ? 0 : spawn_point->y;
    spawn_point->y = sy;

    spawn_point->z = _get_highest_open_block(sx, sy, spawned_object_height);
}

}; // Objects
