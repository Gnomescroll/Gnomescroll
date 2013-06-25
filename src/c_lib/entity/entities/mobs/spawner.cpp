#include "spawner.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/spawner/monster_spawner.hpp>

namespace Entities
{

void load_mob_spawner_data()
{
    EntityType type = ENTITY_MONSTER_SPAWNER;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(Position);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = 1.9f;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::monster_spawner;
    vox->init_hitscan = true;
    vox->init_draw = true;
    vox->frozen = true;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = 150;
    health->health_max = 150;

    auto spawner = ADD_COMPONENT(MonsterSpawner);
    spawner->radius = 2.5f;
    spawner->set_max_children(10);
    spawner->spawn_type = ENTITY_MONSTER_BOX;

    #if DC_SERVER
    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 4, 0.5f);
    item_drop->drop->add_drop("synthesizer_coin", 6, 0.35f);
    item_drop->drop->add_drop("synthesizer_coin", 8, 0.15f);

    item_drop->drop->set_max_drop_amounts("small_charge_pack", 4);
    item_drop->drop->add_drop("small_charge_pack", 1, 0.50f);
    item_drop->drop->add_drop("small_charge_pack", 2, 0.25f);
    item_drop->drop->add_drop("small_charge_pack", 3, 0.15f);
    item_drop->drop->add_drop("small_charge_pack", 4, 0.05f);

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = Color(223, 31, 31);
    anim->count = 5*5*5;
    anim->size = 0.4f;
    #endif
}

void ready_mob_spawner(Entity* entity)
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

void tick_mob_spawner(Entity* entity)
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

} // Entities

