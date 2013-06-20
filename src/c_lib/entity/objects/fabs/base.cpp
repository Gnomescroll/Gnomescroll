#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/fabs/constants.hpp>
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
    dims->height = BASE_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::base;
    vox->init_hitscan = BASE_INIT_WITH_HITSCAN;
    vox->init_draw = BASE_INIT_WITH_DRAW;
    vox->should_hitscan = BASE_SHOULD_HITSCAN;

    #if DC_SERVER
    auto spawner = ADD_COMPONENT(AgentSpawner);
    spawner->radius = BASE_SPAWN_RADIUS;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif
}

void ready_base(Entity* object)
{

    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_base(Entity* object)
{
    GS_ASSERT(false);
}

void tick_base(Entity* object)
{
    #if DC_SERVER
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    IF_ASSERT(physics == NULL) return;
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    physics->set_position(position);
    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, object);
    if (limiter->allowed()) object->broadcastState();
    #endif
}

void update_base(Entity* object)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    Vec3 angles = physics->get_angles();
    vox->force_update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
