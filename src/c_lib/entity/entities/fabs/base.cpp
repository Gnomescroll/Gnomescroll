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
    vox->frozen = true;
    vox->should_hitscan = false;

    #if DC_SERVER
    auto spawner = ADD_COMPONENT(AgentSpawner);
    spawner->radius = BASE_SPAWN_RADIUS;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif
}

void tick_base(Entity* entity)
{
    #if DC_SERVER
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    IF_ASSERT(physics == NULL) return;
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    physics->set_position(position);
    #endif
}

} // Entities
