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

void ready_agent_spawner(Entity* entity)
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

void die_agent_spawner(Entity* entity)
{
    #if DC_SERVER
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, entity);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    // remove self from any agents using us
    using Agents::agent_list;
    for (size_t i=0; i<agent_list->max; i++)
        if (agent_list->objects[i].id != agent_list->null_id)
            if (agent_list->objects[i].status.spawner == entity->id)
                agent_list->objects[i].status.set_spawner(BASE_SPAWN_ID);

    entity->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    if (vox != NULL && vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, entity);
        if (anim != NULL)
            anim->explode_random(vox->get_center());
    }

    //dieChatMessage(entity);
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
