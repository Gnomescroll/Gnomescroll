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

    #if DC_SERVER
    int n_components = 5;
    #endif
    #if DC_CLIENT
    int n_components = 3;
    #endif

    entity_data->set_components(type, n_components);

    entity_data->attach_component(type, COMPONENT_Position);
    entity_data->attach_component(type, COMPONENT_Dimension);
    entity_data->attach_component(type, COMPONENT_VoxelModel);

    #if DC_SERVER
    entity_data->attach_component(type, COMPONENT_AgentSpawner);
    entity_data->attach_component(type, COMPONENT_RateLimit);
    #endif
}

static void set_base_properties(Entity* object)
{
    add_component_to_object(object, COMPONENT_Position);

    auto dims = ADD_COMPONENT(Dimension, object);
    dims->height = BASE_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel, object);
    vox->vox_dat = &VoxDats::base;
    vox->init_hitscan = BASE_INIT_WITH_HITSCAN;
    vox->init_draw = BASE_INIT_WITH_DRAW;
    vox->should_hitscan = BASE_SHOULD_HITSCAN;

    #if DC_SERVER
    auto spawner = ADD_COMPONENT(AgentSpawner, object);
    spawner->radius = BASE_SPAWN_RADIUS;

    auto limiter = ADD_COMPONENT(RateLimit, object);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif

    object->tick = &tick_base;
    object->update = &update_base;

    object->create = create_packet;
    object->state = state_packet;
}

Entity* create_base()
{
    EntityType type = ENTITY_BASE;
    Entity* obj = entity_list->create(type);
    IF_ASSERT(obj == NULL) return NULL;
    set_base_properties(obj);
    return obj;
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
