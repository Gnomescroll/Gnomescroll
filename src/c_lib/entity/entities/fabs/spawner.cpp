#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>
#include <agent/_interface.hpp>

namespace Entities
{

void load_agent_spawner_data()
{
    EntityType type = ENTITY_AGENT_SPAWNER;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(Position);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = 1.9f;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::agent_spawner;
    vox->init_hitscan = true;
    vox->init_draw = true;
    vox->init_frozen = true;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = 200;
    health->health_max = 200;

    #if DC_SERVER
    auto spawner = ADD_COMPONENT(AgentSpawner);
    spawner->radius = 0;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(1);
    item_drop->drop->set_max_drop_amounts("agent_spawner", 1);
    item_drop->drop->add_drop("agent_spawner", 1, 1.0f);
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->count = 35;
    anim->count_max = 50;
    anim->size = 0.22f;
    anim->force = 5.0f;
    anim->color = Color(127, 31, 223);
    #endif
}

void tick_agent_spawner(Entity* entity)
{
    #if DC_SERVER
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    physics->set_position(position);
    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, entity);
    if (limiter->allowed()) entity->broadcastState();
    #endif
}

void update_agent_spawner(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);

    Vec3 angles = physics->get_angles();
    Vec3 pos = physics->get_position();
    vox->force_update(pos, angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
