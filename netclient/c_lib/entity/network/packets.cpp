#include "packets.hpp"

#if DC_CLIENT

#include <c_lib/sound/sound.hpp>
#include <c_lib/animations/_interface.hpp>
#include <c_lib/animations/animations.hpp>

#include <c_lib/agent/client/player_agent.hpp>

#include <c_lib/physics/vec3.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/objects.hpp>

/* Construction */

inline void object_create_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }
    Objects::ready(obj);
}

inline void object_create_momentum_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
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
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    Objects::ready(obj);
}

inline void object_create_owner_team_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::TeamComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;

    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }

    OwnerComponent* owner_component = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    if (owner_component != NULL)
    {
        owner_component->set_owner(owner);
    }
    
    TeamComponent* team_component = (TeamComponent*)obj->get_component_interface(COMPONENT_INTERFACE_TEAM);
    if (team_component != NULL)
    {
        team_component->set_team(team);
    }
    
    Objects::ready(obj);
}

inline void object_create_owner_team_index_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::IndexedTeamComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;

    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }

    OwnerComponent* owner_component = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    if (owner_component != NULL)
    {
        owner_component->set_owner(owner);
    }
    
    IndexedTeamComponent* team_component = (IndexedTeamComponent*)obj->get_component(COMPONENT_INDEXED_TEAM);
    if (team_component != NULL)
    {
        team_component->set_team(team);
        team_component->set_team_index(team_index);
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
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }
}

inline void object_state_momentum_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
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
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
        physics->set_angles(vec3_init(theta, phi, 0));
    }
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
    // not all models are update all the time
    // later, use the was_updated flag (make sure it is being set correctly in agent vox update) TODO
    a->vox->was_updated = false;
    a->vox->update(a->s.x, a->s.y, a->s.z, a->s.theta, a->s.phi);
    Voxel_volume* vv = a->vox->get_part(this->target_part);
    if (vv == NULL) return;
    Vec3 dest = vv->get_center();

    // laser animation
    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_sub(dest, position);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(
        position.x, position.y, position.z,
        v.x, v.y, v.z
    );

    // destroy model
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* weapon = (WeaponTargetingComponent*)obj->get_component(COMPONENT_WEAPON_TARGETING);
    if (weapon != NULL)
    {
        int voxel[3] = { this->voxel_x, this->voxel_y, this->voxel_z };
        destroy_object_voxel(
            this->target_id, this->target_type, this->target_part,
            voxel, weapon->attacker_properties.voxel_damage_radius
        );
    }

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

inline void object_choose_target_StoC::handle()
{
    Objects::Object* obj = Objects::get((ObjectType)this->type, this->id);
    if (obj == NULL) return;

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)obj->get_component(COMPONENT_MOTION_TARGETING);
    if (motion != NULL) motion->en_route = false;  // cancel all motion

    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* weapon = (WeaponTargetingComponent*)obj->get_component(COMPONENT_WEAPON_TARGETING);
    if (weapon != NULL)
    {
        weapon->target_id = this->target_id;    // set target
        weapon->target_type = (ObjectType)this->target_type;
        weapon->locked_on_target = true;   // flag target lock
    }
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
    
    motion->destination = vec3_init(this->x, this->y, this->z);
    motion->ticks_to_destination = this->ticks;

    // set momentum from destination :: TODO MOVE
    Vec3 direction = vec3_sub(motion->destination, physics->get_position());
    if (this->ticks)
    {
        float len = vec3_length(direction);
        float speed = len / ((float)this->ticks);
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

#endif

#if DC_SERVER
inline void object_create_StoC::handle() {}
inline void object_create_momentum_StoC::handle() {}
inline void object_create_momentum_angles_StoC::handle() {}
inline void object_create_owner_team_StoC::handle() {}
inline void object_create_owner_team_index_StoC::handle() {}
inline void object_destroy_StoC::handle() {}
inline void object_picked_up_StoC::handle() {}
inline void object_state_StoC::handle() {}
inline void object_state_momentum_StoC::handle() {}
inline void object_state_momentum_angles_StoC::handle() {}
inline void object_shot_object_StoC::handle() {}
inline void object_shot_terrain_StoC::handle() {}
inline void object_shot_nothing_StoC::handle() {}
inline void object_choose_target_StoC::handle() {}
inline void object_choose_destination_StoC::handle() {}
#endif
