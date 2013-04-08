#include "packets.hpp"

#if DC_CLIENT
#include <sound/sound.hpp>
#include <animations/_interface.hpp>
#include <animations/animations.hpp>
#include <agent/client/player_agent.hpp>
#include <physics/vec3.hpp>
#include <entity/constants.hpp>
#include <entity/objects.hpp>
#include <particle/_interface.hpp>
#include <particle/text/billboard_text.hpp>
#include <particle/constants.hpp>

/* Construction */

inline void object_create_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;
    IF_ASSERT(type >= MAX_OBJECT_TYPES) return;
    IF_ASSERT(id >= GAME_OBJECTS_MAX) return;

    Entity* obj = Entities::create((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL) physics->set_position(this->position);
    Entities::ready(obj);
}

inline void object_create_owner_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    IF_ASSERT(type >= MAX_OBJECT_TYPES) return;
    IF_ASSERT(id >= GAME_OBJECTS_MAX) return;

    Entity* obj = Entities::create((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL) physics->set_position(this->position);
    OwnerComponent* owner = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    GS_ASSERT(owner != NULL);
    if (owner != NULL) owner->set_owner((AgentID)this->owner);
    Entities::ready(obj);
}

inline void object_create_momentum_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;

    Entity* obj = Entities::create((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(this->position);
        physics->set_momentum(this->momentum);
    }
    Entities::ready(obj);
}

inline void object_create_momentum_angles_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;

    Entity* obj = Entities::create((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(this->position);
        physics->set_momentum(this->momentum);
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    Entities::ready(obj);
}

inline void object_create_momentum_angles_health_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;
    using Components::HitPointsHealthComponent;

    Entity* obj = Entities::create((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(this->position);
        physics->set_momentum(this->momentum);
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)obj->get_component(COMPONENT_HIT_POINTS);
    GS_ASSERT(health != NULL);
    if (health != NULL)
    {
        health->health_max = this->health_max;
        health->health = this->health_max;
    }
    Entities::ready(obj);
}

/* State */

inline void object_state_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;

    Entity* obj = Entities::get((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;
    physics->set_position(this->position);
}

inline void object_state_momentum_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;

    Entity* obj = Entities::get((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;
    physics->set_position(this->position);
    physics->set_momentum(this->momentum);
}

inline void object_state_momentum_angles_StoC::handle()
{
    using Entities::Entity;
    using Components::PhysicsComponent;

    Entity* obj = Entities::get((EntityType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    IF_ASSERT(physics == NULL) return;

    physics->set_position(this->position);
    physics->set_momentum(this->momentum);
    physics->set_angles(vec3_init(theta, phi, 0));
}

inline void object_state_health_StoC::handle()
{
    using Entities::Entity;
    using Components::HitPointsHealthComponent;
    Entity* obj = Entities::get((EntityType)type, id);
    if (obj == NULL) return;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)obj->get_component(COMPONENT_HIT_POINTS);
    IF_ASSERT(health == NULL) return;
    health->health = this->health;
}

/* Destruction */

inline void object_destroy_StoC::handle()
{
    Entities::destroy((EntityType)type, id);
}

/* Actions */

/* Hitscan */

inline void object_shot_object_StoC::handle()
{
    if (this->target_type != OBJECT_AGENT) return; // remove this once turret can attack other objects

    // get firing object
    Entities::Entity* obj = Entities::get((EntityType)this->type, (int)this->id);
    if (obj == NULL) return;

    // get firing position of object
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)obj->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (dims != NULL) position.z += dims->get_camera_height();

    // get target
    Agents::Agent* a = Agents::get_agent((AgentID)this->target_id);
    if (a == NULL || a->vox == NULL) return;
    // update the model, in case it is out of date.
    force_update_agent_vox(a);

    Vec3 dest = a->vox->get_center(this->target_part);
    dest = quadrant_translate_position(position, dest);

    // laser animation
    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_sub(dest, position);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(position, v);
    Sound::play_3d_sound("turret_shoot", position);
}

inline void object_shot_terrain_StoC::handle()
{
    Entities::Entity* obj = Entities::get((EntityType)this->type, this->id);
    if (obj == NULL) return;

    // get firing position of object
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)obj->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (dims != NULL) position.z += dims->get_camera_height();

    Vec3 dest = quadrant_translate_position(position, this->destination);
    Vec3 v = vec3_sub(dest, position);
    v = vec3_normalize(v);
    const float hitscan_effect_speed = 200.0f;
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(position, v);
    Animations::block_damage(dest, position,
                             (CubeType)this->cube, this->side);
    Animations::particle_explode(dest);
    //Sound::play_3d_sound("laser_hit_block", dest);
    Sound::play_3d_sound("turret_shoot", position);
}

inline void object_shot_nothing_StoC::handle()
{
    Entities::Entity* obj = Entities::get((EntityType)this->type, this->id);
    if (obj == NULL) return;

    // get firing position of object
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)obj->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (dims != NULL) position.z += dims->get_camera_height();

    struct Vec3 v = vec3_normalize(this->direction);
    const float hitscan_effect_speed = 200.0f;
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(position, v);
    Sound::play_3d_sound("turret_shoot", position);
}

inline void object_took_damage_StoC::handle()
{
    Entities::Entity* obj = Entities::get((EntityType)this->type, this->id);
    if (obj == NULL) return;

    // get object position
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    // put position at top of object
    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)obj->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (dims != NULL) position.z += dims->get_height();

    // get radial range to place damage particle within
    float radius = 0.0f;
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)obj->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox != NULL) radius = vox->get_radius();

    // create billboard text at position
    Particle::BillboardText* b = Particle::billboard_text_list->create();
    if (b == NULL) return;
    b->set_state(position.x + (radius * (2*randf() - 1)),
                 position.y + (radius * (2*randf() - 1)),
                 position.z,
                 0.0f, 0.0f, Particle::BB_PARTICLE_DMG_VELOCITY_Z);
    b->set_color(Particle::BB_PARTICLE_DMG_COLOR);   // red
    const size_t txtlen = 11;
    char txt[txtlen + 1];
    snprintf(txt, txtlen + 1, "%d", this->damage);
    txt[txtlen] = '\0';
    b->set_text(txt);
    b->set_scale(1.0f);
    b->set_ttl(245);
}

#endif

#if DC_SERVER
inline void object_create_StoC::handle() {}
inline void object_create_owner_StoC::handle() {}
inline void object_create_momentum_StoC::handle() {}
inline void object_create_momentum_angles_StoC::handle() {}
inline void object_create_momentum_angles_health_StoC::handle() {}
inline void object_destroy_StoC::handle() {}
inline void object_state_StoC::handle() {}
inline void object_state_momentum_StoC::handle() {}
inline void object_state_momentum_angles_StoC::handle() {}
inline void object_shot_object_StoC::handle() {}
inline void object_shot_terrain_StoC::handle() {}
inline void object_shot_nothing_StoC::handle() {}
inline void object_took_damage_StoC::handle() {}
inline void object_state_health_StoC::handle() {}
#endif
