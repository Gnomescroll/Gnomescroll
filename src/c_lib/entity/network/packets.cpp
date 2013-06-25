#include "packets.hpp"

#if DC_CLIENT
#include <sound/sound.hpp>
#include <animations/_interface.hpp>
#include <animations/animations.hpp>
#include <agent/client/player_agent.hpp>
#include <physics/vec3.hpp>
#include <entity/constants.hpp>
#include <entity/entities.hpp>
#include <particle/_interface.hpp>
#include <particle/text/billboard_text.hpp>
#include <particle/constants.hpp>

/* Construction */

inline void entity_create_StoC::handle()
{
    using Entities::Entity;
    IF_ASSERT(type >= MAX_ENTITY_TYPES) return;
    IF_ASSERT(id >= GAME_OBJECTS_MAX) return;

    Entity* obj = Entities::create(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    GS_ASSERT(physics != NULL);
    if (physics != NULL) physics->set_position(this->position);
    Entities::ready(obj);
}

inline void entity_create_owner_StoC::handle()
{
    using Entities::Entity;
    IF_ASSERT(type >= MAX_ENTITY_TYPES) return;
    IF_ASSERT(id >= GAME_OBJECTS_MAX) return;

    Entity* obj = Entities::create(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    GS_ASSERT(physics != NULL);
    if (physics != NULL) physics->set_position(this->position);
    auto owner = GET_COMPONENT_INTERFACE(Owner, obj);
    GS_ASSERT(owner != NULL);
    if (owner != NULL) owner->set_owner((AgentID)this->owner);
    Entities::ready(obj);
}

inline void entity_create_momentum_StoC::handle()
{
    using Entities::Entity;

    Entity* obj = Entities::create(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(this->position);
        physics->set_momentum(this->momentum);
    }
    Entities::ready(obj);
}

inline void entity_create_momentum_angles_StoC::handle()
{
    using Entities::Entity;

    Entity* obj = Entities::create(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(this->position);
        physics->set_momentum(this->momentum);
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    Entities::ready(obj);
}

inline void entity_create_momentum_angles_health_StoC::handle()
{
    using Entities::Entity;

    Entity* obj = Entities::create(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(this->position);
        physics->set_momentum(this->momentum);
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    auto health = GET_COMPONENT(HitPoints, obj);
    GS_ASSERT(health != NULL);
    if (health != NULL)
    {
        health->health_max = this->health_max;
        health->health = this->health_max;
    }
    Entities::ready(obj);
}

/* State */

inline void entity_state_StoC::handle()
{
    using Entities::Entity;

    Entity* obj = Entities::get(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    IF_ASSERT(physics == NULL) return;
    physics->set_position(this->position);
}

inline void entity_state_momentum_StoC::handle()
{
    using Entities::Entity;

    Entity* obj = Entities::get(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    IF_ASSERT(physics == NULL) return;
    physics->set_position(this->position);
    physics->set_momentum(this->momentum);
}

inline void entity_state_momentum_angles_StoC::handle()
{
    using Entities::Entity;

    Entity* obj = Entities::get(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    IF_ASSERT(physics == NULL) return;

    physics->set_position(this->position);
    physics->set_momentum(this->momentum);
    physics->set_angles(vec3_init(theta, phi, 0));
}

inline void entity_state_health_StoC::handle()
{
    using Entities::Entity;
    Entity* obj = Entities::get(EntityType(type), EntityID(id));
    if (obj == NULL) return;
    auto health = GET_COMPONENT(HitPoints, obj);
    IF_ASSERT(health == NULL) return;
    health->health = this->health;
}

/* Destruction */

inline void entity_destroy_StoC::handle()
{
    Entities::destroy(EntityType(type), EntityID(id));
}

/* Actions */

/* Hitscan */

inline void entity_hitscan_entity_StoC::handle()
{
    if (this->target_type != ENTITY_AGENT) return; // remove this once turret can attack other entities

    // get firing entity
    Entities::Entity* obj = Entities::get(EntityType(this->type), EntityID(this->id));
    if (obj == NULL) return;

    // get firing position of entity
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    auto dims = GET_COMPONENT_INTERFACE(Dimension, obj);
    if (dims != NULL) position.z += dims->get_camera_height();

    // get target
    Agents::Agent* a = Agents::get_agent((AgentID)this->target_id);
    if (a == NULL || a->vox == NULL) return;
    // update the model, in case it is out of date.
    force_update_agent_vox(a);

    Vec3 dest = a->get_center();
    dest = quadrant_translate_position(position, dest);

    // laser animation
    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_sub(dest, position);
    v = vec3_normalize(v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(position, v);
    Sound::play_3d_sound("turret_shoot", position);
}

inline void entity_hitscan_terrain_StoC::handle()
{
    Entities::Entity* obj = Entities::get(EntityType(this->type), EntityID(this->id));
    if (obj == NULL) return;

    // get firing position of entity
    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    auto dims = GET_COMPONENT_INTERFACE(Dimension, obj);
    if (dims != NULL) position.z += dims->get_camera_height();

    Vec3 destination = vec3_scalar_add(vec3_init(this->position), 0.5f);
    Vec3 trans_dest = quadrant_translate_position(position, destination);
    Vec3 v = vec3_sub(trans_dest, position);
    v = vec3_normalize(v);
    const float hitscan_effect_speed = 200.0f;
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(position, v);
    CubeType cube = t_map::get(this->position);
    int side = side_orientation(position, destination);
    Animations::block_damage(destination, position, cube, side);
    Animations::particle_explode(destination);
    //Sound::play_3d_sound("laser_hit_block", dest);
    Sound::play_3d_sound("turret_shoot", position);
}

inline void entity_hitscan_nothing_StoC::handle()
{
    Entities::Entity* obj = Entities::get(EntityType(this->type), EntityID(this->id));
    if (obj == NULL) return;

    Vec3 position;
    auto dims = GET_COMPONENT_INTERFACE(Dimension, obj);
    if (dims == NULL)
    {
        auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
        if (physics == NULL) return;
        position = physics->get_position();
    }
    else
        position = dims->get_camera_position();

    Vec3 v = vec3_init(0, 0, 1);
    if (vec3_length_squared(this->direction) != 0)
        v = vec3_normalize(this->direction);
    const float hitscan_effect_speed = 200.0f;
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(position, v);
    Sound::play_3d_sound("turret_shoot", position);
}

inline void entity_took_damage_StoC::handle()
{
    Entities::Entity* obj = Entities::get(EntityType(this->type), EntityID(this->id));
    if (obj == NULL) return;

    auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    auto dims = GET_COMPONENT_INTERFACE(Dimension, obj);
    float height = 1.0f;
    if (dims != NULL)
    {
        height = dims->get_height();
        position.z += height / 2.0f;
    }

    float radius = 0.0f;
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, obj);
    if (vox != NULL) radius = vox->get_radius();

    BoundingBox box;
    box.radius = radius;
    box.height = height;
    Animations::create_health_change_indicator(box, position, -int(damage));
}

#endif

#if DC_SERVER
inline void entity_create_StoC::handle() {}
inline void entity_create_owner_StoC::handle() {}
inline void entity_create_momentum_StoC::handle() {}
inline void entity_create_momentum_angles_StoC::handle() {}
inline void entity_create_momentum_angles_health_StoC::handle() {}
inline void entity_destroy_StoC::handle() {}
inline void entity_state_StoC::handle() {}
inline void entity_state_momentum_StoC::handle() {}
inline void entity_state_momentum_angles_StoC::handle() {}
inline void entity_hitscan_entity_StoC::handle() {}
inline void entity_hitscan_terrain_StoC::handle() {}
inline void entity_hitscan_nothing_StoC::handle() {}
inline void entity_took_damage_StoC::handle() {}
inline void entity_state_health_StoC::handle() {}
#endif
