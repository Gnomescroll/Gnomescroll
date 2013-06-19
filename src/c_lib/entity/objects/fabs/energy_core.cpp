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

    #if DC_SERVER
    int n_components = 7;
    #endif
    #if DC_CLIENT
    int n_components = 5;
    #endif

    entity_data->set_components(type, n_components);

    entity_data->attach_component(type, COMPONENT_Position);
    entity_data->attach_component(type, COMPONENT_Dimension);
    entity_data->attach_component(type, COMPONENT_VoxelModel);
    entity_data->attach_component(type, COMPONENT_HitPoints);

    #if DC_SERVER
    entity_data->attach_component(type, COMPONENT_Healer);
    entity_data->attach_component(type, COMPONENT_ItemDrop);
    entity_data->attach_component(type, COMPONENT_RateLimit);
    #endif

    #if DC_CLIENT
    entity_data->attach_component(type, COMPONENT_Animation);
    #endif
}

static void set_energy_core_properties(Entity* object)
{
    void* ret = (void*)ADD_COMPONENT(Position, object);
    GS_ASSERT(ret != NULL);

    auto dims = ADD_COMPONENT(Dimension, object);
    dims->height = ENERGY_CORE_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel, object);
    vox->vox_dat = &VoxDats::energy_core;
    vox->init_hitscan = ENERGY_CORE_INIT_WITH_HITSCAN;
    vox->init_draw = ENERGY_CORE_INIT_WITH_DRAW;

    auto health = ADD_COMPONENT(HitPoints, object);
    health->health = ENERGY_CORE_MAX_HEALTH;
    health->health_max = ENERGY_CORE_MAX_HEALTH;

    #if DC_SERVER
    auto healer = ADD_COMPONENT(Healer, object);
    healer->radius = ENERGY_CORE_HEALING_RADIUS;

    auto limiter = ADD_COMPONENT(RateLimit, object);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop, object);
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
    auto anim = ADD_COMPONENT(Animation, object);
    anim->count = ENERGY_CORE_ANIMATION_COUNT;
    anim->count_max = ENERGY_CORE_ANIMATION_COUNT_MAX;
    anim->size = ENERGY_CORE_ANIMATION_SIZE;
    anim->force = ENERGY_CORE_ANIMATION_FORCE;
    anim->color = ENERGY_CORE_ANIMATION_COLOR;
    #endif

    object->tick = &tick_energy_core;
    object->update = &update_energy_core;

    object->create = create_packet;
    object->state = state_packet;
}

Entity* create_energy_core()
{
    EntityType type = ENTITY_ENERGY_CORE;
    Entity* obj = entity_list->create(type);
    GS_ASSERT(obj != NULL);
    if (obj == NULL) return NULL;
    set_energy_core_properties(obj);
    return obj;
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
