#include "agent_spawner.hpp"

#include <physics/vec3.hpp>
#include <t_map/t_map.hpp>
#include <entity/components/physics.hpp>
#include <agent/agent_physics.hpp>

namespace Components
{

struct Vec3 AgentSpawnerComponent::get_spawn_point(float spawned_object_height, float spawned_object_radius)
{
    GS_ASSERT(spawned_object_radius * 2 < 1.0f);
    Vec3 spawn_point = vec3_init(0,0,0);
    auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
    IF_ASSERT(physics == NULL) return spawn_point;
    Vec3 position = physics->get_position();

    int sx = int(position.x) + randrange(-this->radius, this->radius);
    spawn_point.x = float(translate_point(sx)) + 0.5f;
    int sy = int(position.y) + randrange(-this->radius, this->radius);
    spawn_point.y = float(translate_point(sy)) + 0.5f;

    int h = ceilf(spawned_object_height);
    IF_ASSERT(h < 1) h = 1;
    spawn_point.z = t_map::get_nearest_surface_block(sx, sy, position.z, h);
    return spawn_point;
}

}; // Components
