#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>
#if DC_SERVER
# include <entity/components/healer.hpp>
#endif

namespace Entities
{

void load_energy_core_data()
{
    EntityType type = ENTITY_ENERGY_CORE;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(Position);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = ENERGY_CORE_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::energy_core;
    vox->init_hitscan = ENERGY_CORE_INIT_WITH_HITSCAN;
    vox->init_draw = ENERGY_CORE_INIT_WITH_DRAW;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = ENERGY_CORE_MAX_HEALTH;
    health->health_max = ENERGY_CORE_MAX_HEALTH;

    #if DC_SERVER
    auto healer = ADD_COMPONENT(Healer);
    healer->radius = ENERGY_CORE_HEALING_RADIUS;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("energy_tank", 2);
    item_drop->drop.add_drop("energy_tank", 2, 0.5f);
    item_drop->drop.add_drop("energy_tank", 3, 0.5f);
    item_drop->drop.set_max_drop_amounts("small_charge_pack", 3);
    item_drop->drop.add_drop("small_charge_pack", 4, 0.5f);
    item_drop->drop.add_drop("small_charge_pack", 5, 0.3f);
    item_drop->drop.add_drop("small_charge_pack", 6, 0.2f);
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->count = ENERGY_CORE_ANIMATION_COUNT;
    anim->count_max = ENERGY_CORE_ANIMATION_COUNT_MAX;
    anim->size = ENERGY_CORE_ANIMATION_SIZE;
    anim->force = ENERGY_CORE_ANIMATION_FORCE;
    anim->color = ENERGY_CORE_ANIMATION_COLOR;
    #endif
}

void ready_energy_core(Entity* object)
{

    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    GS_ASSERT(vox != NULL);
    GS_ASSERT(physics != NULL);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_energy_core(Entity* object)
{
    #if DC_SERVER
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, object);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    object->broadcastDeath();
    #endif

    #if DC_CLIENT
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    GS_ASSERT(vox != NULL);
    if (vox != NULL && vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, object);
        GS_ASSERT(anim != NULL);
        if (anim != NULL)
            anim->explode_random(vox->get_center());
    }
    #endif
}

void tick_energy_core(Entity* object)
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

void update_energy_core(Entity* object)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);

    Vec3 angles = physics->get_angles();
    Vec3 pos = physics->get_position();
    vox->force_update(pos, angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
