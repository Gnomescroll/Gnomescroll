#include "spawner.hpp"

namespace Components
{

void AgentSpawnerComponent::get_spawn_point(Vec3 position, int spawned_object_height, Vec3* spawn_point)
{
    int x,y;
    x = (int)position.x;
    y = (int)position.y;

    int sx,sy;
    sx = randrange(x - this->radius, x + this->radius);
    sy = randrange(y - this->radius, y + this->radius);
    spawn_point.x = (sx < map_dim.x) ? sx : map_dim.x;
    spawn_point.x = (spawn_point.x < 0) ? 0 : spawn_point.x;
    spawn_point.y = (sy < map_dim.y) ? sy : map_dim.y;
    spawn_point.y = (spawn_point.y < 0) ? 0 : spawn_point.y;
    spawn_point.z = _get_highest_open_block(spawn_point.x, spawn_point.y, spawned_object_height);
}

}; // Objects
