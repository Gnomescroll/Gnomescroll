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
    using Objects::Object;
    using Components::PhysicsComponent;

    GS_ASSERT(type < MAX_OBJECT_TYPES);
    GS_ASSERT(id < GAME_OBJECTS_MAX);
    if (type >= MAX_OBJECT_TYPES) return;
    if (id >= GAME_OBJECTS_MAX) return;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL) physics->set_position(vec3_init(x,y,z));
    Objects::ready(obj);
}

inline void object_create_owner_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;
    using Components::OwnerComponent;

    GS_ASSERT(type < MAX_OBJECT_TYPES);
    GS_ASSERT(id < GAME_OBJECTS_MAX);
    if (type >= MAX_OBJECT_TYPES) return;
    if (id >= GAME_OBJECTS_MAX) return;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL) physics->set_position(vec3_init(x,y,z));
    OwnerComponent* owner = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    GS_ASSERT(owner != NULL);
    if (owner != NULL) owner->set_owner(this->owner);
    Objects::ready(obj);
}

inline void object_create_momentum_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
    }
    Objects::ready(obj);
}

inline void object_create_momentum_angles_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    Objects::ready(obj);
}

inline void object_create_momentum_angles_health_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;
    using Components::HitPointsHealthComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)obj->get_component(COMPONENT_HIT_POINTS);
    GS_ASSERT(health != NULL);
    if (health != NULL)
    {
        health->max_health = this->max_health;
        health->health = this->max_health;
    }
    Objects::ready(obj);
}

/* State */

inline void object_state_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL) physics->set_position(vec3_init(x,y,z));
}

inline void object_state_momentum_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
    }
}

inline void object_state_momentum_angles_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
        physics->set_angles(vec3_init(theta, phi, 0));
    }
}

inline void object_state_health_StoC::handle()
{
    using Objects::Object;
    using Components::HitPointsHealthComponent;
    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)obj->get_component(COMPONENT_HIT_POINTS);
    GS_ASSERT(health != NULL);
    if (health != NULL) health->health = this->health;
}

/* Destruction */

inline void object_destroy_StoC::handle()
{
    Objects::destroy((ObjectType)type, id);
}

/* Actions */

inline void object_picked_up_StoC::handle()
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL && playerAgent_state.you->id == agent_id)
        Sound::pickup_item();
    Objects::destroy((ObjectType)type, id);
}

/* Hitscan */

inline void object_shot_object_StoC::handle()
{
    if (this->target_type != OBJECT_AGENT) return; // remove this once turret can attack other objects

    // get firing object
    Objects::Object* obj = Objects::get((ObjectType)this->type, (int)this->id);
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
    Agent_state* a = ClientState::agent_list->get(this->target_id);
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
    Animations::create_hitscan_effect(
        position.x, position.y, position.z,
        v.x, v.y, v.z
    );

    // todo -- sound event
    Sound::turret_shoot(position.x, position.y, position.z, 0,0,0);
}

inline void object_shot_terrain_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;

    // get firing position of object
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)obj->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (dims != NULL) position.z += dims->get_camera_height();

    Vec3 dest = vec3_init(this->x, this->y, this->z);
    dest = quadrant_translate_position(position, dest);
    Vec3 v = vec3_sub(dest, position); 
    normalize_vector(&v);
    const float hitscan_effect_speed = 200.0f;
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(
        position.x, position.y, position.z,
        v.x, v.y, v.z
    );

    Animations::block_damage(
        this->x, this->y, this->z, position.x, position.y, position.z,
        this->cube, this->side
    );
    Animations::terrain_sparks(this->x, this->y, this->z);
    Sound::laser_hit_block(this->x, this->y, this->z, 0,0,0);
    Sound::turret_shoot(position.x, position.y, position.z, 0,0,0);
}

inline void object_shot_nothing_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;

    // get firing position of object
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)obj->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (dims != NULL) position.z += dims->get_camera_height();

    Vec3 v = vec3_init(this->x, this->y, this->z);
    normalize_vector(&v);
    const float hitscan_effect_speed = 200.0f;
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(
        position.x, position.y, position.z,
        v.x, v.y, v.z
    );
    Sound::turret_shoot(position.x, position.y, position.z, 0,0,0);
}

inline void object_choose_weapon_target_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;

    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* weapon = (WeaponTargetingComponent*)obj->get_component(COMPONENT_WEAPON_TARGETING);
    GS_ASSERT(weapon != NULL);
    if (weapon != NULL)
    {
        weapon->target_id = this->target_id;    // set target
        weapon->target_type = (ObjectType)this->target_type;
        weapon->locked_on_target = true;   // flag target lock
    }

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)obj->get_component(COMPONENT_MOTION_TARGETING);
    if (motion != NULL) motion->en_route = false;  // cancel all motion
}

inline void object_choose_motion_target_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)obj->get_component(COMPONENT_MOTION_TARGETING);
    GS_ASSERT(motion != NULL);
    if (motion == NULL) return;
    motion->set_target((ObjectType)target_type, target_id);
}

inline void object_remove_motion_target_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)obj->get_component(COMPONENT_MOTION_TARGETING);
    GS_ASSERT(motion != NULL);
    if (motion == NULL) return;
    motion->target_type = OBJECT_NONE;
    motion->target_id = NO_AGENT;
}

inline void object_choose_destination_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;
    
    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)obj->get_component(COMPONENT_MOTION_TARGETING);
    if (motion == NULL) return;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    Vec3 position = physics->get_position();
    
    Vec3 destination = vec3_init(this->x, this->y, this->z);
    motion->destination = quadrant_translate_position(position, destination);
    motion->ticks_to_destination = this->ticks_to_destination;
    
    // set momentum from destination :: TODO MOVE
    Vec3 direction = vec3_sub(motion->destination, position);
    if (this->ticks_to_destination)
    {
        float len = vec3_length(direction);
        float speed = len / ((float)this->ticks_to_destination);
        motion->speed = speed;
        motion->at_destination = false;
        motion->en_route = true;
        if (len)
        {
            normalize_vector(&direction);
            motion->target_direction = direction;
        }
    }
    else
    {   // no ticks is equivalent to a teleport
        physics->set_position(motion->destination);
        motion->speed = 0.0f;
        motion->at_destination = true;
        motion->en_route = false;
    }

    // cancel target
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* weapon = (WeaponTargetingComponent*)obj->get_component(COMPONENT_WEAPON_TARGETING);
    if (weapon != NULL) weapon->locked_on_target = false;
}

inline void object_took_damage_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
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
    b->init();
    b->set_state(
        position.x + (radius * (2*randf() - 1)),
        position.y + (radius * (2*randf() - 1)),
        position.z,
        0.0f,0.0f, BB_PARTICLE_DMG_VELOCITY_Z
    );
    b->set_color(BB_PARTICLE_DMG_COLOR);   // red
    char txt[10+1];
    sprintf(txt, "%d", this->damage);
    b->set_text(txt);
    b->set_scale(1.0f);
    b->set_ttl(245);
}

inline void object_begin_waiting_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;
    
    using Components::StateMachineComponent;
    StateMachineComponent* machine = (StateMachineComponent*)
        obj->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    if (machine == NULL) return;
    if (machine->router != NULL)
        machine->router(obj, STATE_WAITING);
}

inline void object_in_transit_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    GS_ASSERT(obj != NULL);
    if (obj == NULL) return;

    using Components::DestinationTargetingComponent;
    DestinationTargetingComponent* dest_target = (DestinationTargetingComponent*)
        obj->get_component(COMPONENT_DESTINATION_TARGETING);
    GS_ASSERT(dest_target != NULL);
    if (dest_target == NULL) return;
    
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)
        obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;
    struct Vec3 pos = physics->get_position();
    
    dest_target->ticks_to_destination = this->ticks_to_destination;

    dest_target->set_destination(destination);
    dest_target->orient_to_target(pos);
    
    if (this->ticks_to_destination)
    {
        destination = quadrant_translate_position(pos, destination);
        Vec3 direction = vec3_sub(destination, pos);
        float len = vec3_length(direction);
        dest_target->speed = len / ((float)this->ticks_to_destination);
    }
    else
    {   // 0 ticks is teleport
        if (physics != NULL) physics->set_position(destination);
        dest_target->speed = 0.0f;
        dest_target->at_destination = true;
    }
    
    using Components::StateMachineComponent;
    StateMachineComponent* machine = (StateMachineComponent*)
        obj->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    GS_ASSERT(machine != NULL);
    if (machine == NULL) return;
    if (machine->router != NULL)
        machine->router(obj, STATE_IN_TRANSIT);
}

inline void object_chase_agent_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;
    
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)
        obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    using Components::AgentTargetingComponent;
    AgentTargetingComponent* target = (AgentTargetingComponent*)
        obj->get_component(COMPONENT_AGENT_TARGETING);
    if (target == NULL) return;
    target->target_id = this->target_id;
    target->target_type = OBJECT_AGENT;
    if (physics != NULL)
        target->orient_to_target(physics->get_position());
    
    using Components::StateMachineComponent;
    StateMachineComponent* machine = (StateMachineComponent*)
        obj->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    if (machine == NULL) return;
    if (machine->router != NULL)
        machine->router(obj, STATE_CHASE_AGENT);
}

#endif

#if DC_SERVER
inline void object_create_StoC::handle() {}
inline void object_create_owner_StoC::handle() {}
inline void object_create_momentum_StoC::handle() {}
inline void object_create_momentum_angles_StoC::handle() {}
inline void object_create_momentum_angles_health_StoC::handle() {}
inline void object_destroy_StoC::handle() {}
inline void object_picked_up_StoC::handle() {}
inline void object_state_StoC::handle() {}
inline void object_state_momentum_StoC::handle() {}
inline void object_state_momentum_angles_StoC::handle() {}
inline void object_shot_object_StoC::handle() {}
inline void object_shot_terrain_StoC::handle() {}
inline void object_shot_nothing_StoC::handle() {}
inline void object_choose_motion_target_StoC::handle() {}
inline void object_remove_motion_target_StoC::handle() {}
inline void object_choose_weapon_target_StoC::handle() {}
inline void object_choose_destination_StoC::handle() {}
inline void object_took_damage_StoC::handle() {}
inline void object_state_health_StoC::handle() {}
inline void object_begin_waiting_StoC::handle() {}
inline void object_in_transit_StoC::handle() {}
inline void object_chase_agent_StoC::handle() {}
#endif
