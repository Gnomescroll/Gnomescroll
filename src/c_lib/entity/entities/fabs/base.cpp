#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>

namespace Entities
{

void load_base_data()
{
    EntityType type = ENTITY_BASE;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(Position);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = 1.0f;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::base;
    vox->init_hitscan = false;
    vox->init_draw = true;
    vox->should_hitscan = false;

    #if DC_SERVER
    auto spawner = ADD_COMPONENT(AgentSpawner);
    spawner->radius = BASE_SPAWN_RADIUS;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif
}

void ready_base(Entity* entity)
{

    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    entity->broadcastCreate();
    #endif
}

void die_base(Entity* entity)
{
    GS_ASSERT(false);
}

void tick_base(Entity* entity)
{
    #if DC_SERVER
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    IF_ASSERT(physics == NULL) return;
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    physics->set_position(position);
    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, entity);
    if (limiter->allowed()) entity->broadcastState();
    #endif
}

void update_base(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    Vec3 angles = physics->get_angles();
    vox->force_update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
