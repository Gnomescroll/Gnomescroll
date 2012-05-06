#include "box.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#include <entity/components/targeting.hpp>
#include <entity/components/targeting/weapon_targeting.hpp>
#include <entity/components/targeting/motion_targeting.hpp>

namespace Objects
{

void load_mob_robot_box_data()
{
    ObjectType type = OBJECT_MONSTER_BOX;
    
    #if DC_SERVER
    const int n_components = 7;
    #endif
    #if DC_CLIENT
    const int n_components = 7;
    #endif
    
    object_data->set_components(type, n_components);

    object_data->attach_component(type, COMPONENT_POSITION_MOMENTUM_CHANGED);
    object_data->attach_component(type, COMPONENT_DIMENSION);
    object_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    object_data->attach_component(type, COMPONENT_HIT_POINTS);
    object_data->attach_component(type, COMPONENT_WEAPON_TARGETING);
    object_data->attach_component(type, COMPONENT_MOTION_TARGETING);

    #if DC_SERVER
    object_data->attach_component(type, COMPONENT_SPAWN_CHILD);
    #endif
    #if DC_CLIENT
    object_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_mob_robot_box_properties(Object* object)
{
    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM_CHANGED);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = MONSTER_BOX_HEIGHT;
    dims->camera_height = MONSTER_BOX_CAMERA_HEIGHT;

    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::robot_box;
    vox->init_hitscan = MONSTER_BOX_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_BOX_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    health->health = MONSTER_BOX_MAX_HEALTH;
    health->max_health = MONSTER_BOX_MAX_HEALTH;
    
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* target = (WeaponTargetingComponent*)add_component_to_object(object, COMPONENT_WEAPON_TARGETING);
    target->target_acquisition_probability = MONSTER_BOX_TARGET_ACQUISITION_PROBABILITY;
    target->fire_rate_limit = MONSTER_BOX_FIRE_RATE_LIMIT;
    target->uses_bias = MONSTER_BOX_USES_BIAS;
    target->accuracy_bias = MONSTER_BOX_ACCURACY_BIAS;
    target->sight_range = MONSTER_BOX_FIRING_SIGHT_RANGE;
    target->attacks_enemies = MONSTER_BOX_ATTACK_ONLY_ENEMIES;
    target->attack_at_random = MONSTER_BOX_ATTACK_AT_RANDOM;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = OBJECT_MONSTER_BOX;
    target->attacker_properties.block_damage = MONSTER_BOX_TERRAIN_DAMAGE;
    target->attacker_properties.agent_damage = MONSTER_BOX_AGENT_DAMAGE;
    target->attacker_properties.voxel_damage_radius = MONSTER_BOX_VOXEL_DAMAGE_RADIUS;
    target->attacker_properties.agent_protection_duration = MONSTER_BOX_AGENT_IMMUNITY_DURATION;
    target->attacker_properties.terrain_modification_action = t_map::TMA_MONSTER_BOX;

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)add_component_to_object(object, COMPONENT_MOTION_TARGETING);
    motion->speed = MONSTER_BOX_SPEED;
    motion->max_z_down = MONSTER_BOX_MOTION_MAX_Z_DOWN;
    motion->max_z_up = MONSTER_BOX_MOTION_MAX_Z_UP;

    #if DC_SERVER
    add_component_to_object(object, COMPONENT_SPAWN_CHILD);
    #endif

    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->color = MONSTER_BOX_ANIMATION_COLOR;
    anim->count = MONSTER_BOX_ANIMATION_COUNT;
    //anim->count_max = MONSTER_BOX_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_BOX_ANIMATION_SIZE;
    anim->force = MONSTER_BOX_ANIMATION_FORCE;
    #endif

    object->tick = &tick_mob_robot_box;
    object->update = &update_mob_robot_box;

    object->create = create_packet_momentum_angles;
    object->state = state_packet_momentum_angles;
}

Object* create_mob_robot_box()
{
    ObjectType type = OBJECT_MONSTER_BOX;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_robot_box_properties(obj);
    return obj;
}

void ready_mob_robot_box(Object* object)
{
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* target = (WeaponTargetingComponent*)object->get_component_interface(COMPONENT_INTERFACE_TARGETING);
    target->attacker_properties.id = object->id;
    
    using Components::VoxelModelComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_mob_robot_box(Object* object)
{
    #if DC_SERVER
    //boxDropItem(); -- todo, item drop component
    object->broadcastDeath();

    using Components::SpawnChildComponent;
    SpawnChildComponent* child = (SpawnChildComponent*)object->get_component(COMPONENT_SPAWN_CHILD);
    if (child != NULL) child->notify_parent_of_death();
    #endif

    #if DC_CLIENT
    // explosion animation
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox->vox != NULL)
    {
        using Components::AnimationComponent;
        AnimationComponent* anim = (AnimationComponent*)object->get_component_interface(COMPONENT_INTERFACE_ANIMATION);
        anim->explode(vox->get_center());
    }
    #endif
}

#if DC_SERVER
void server_tick_mob_robot_box(Object* object)
{
    return;
    // must stay on ground -- apply terrain collision
    // wander randomly (TODO: network model with destinations)
    // TODO -- aggro component

    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    const Vec3 position = physics->get_position();
    Vec3 camera_position = position;
    
    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    camera_position.z += dims->get_camera_height();
    
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* weapon = (WeaponTargetingComponent*)object->get_component(COMPONENT_WEAPON_TARGETING);
    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)object->get_component(COMPONENT_MOTION_TARGETING);

    // save current target state, will use weapon to decide if need to send packet
    bool was_on_target = weapon->locked_on_target;
    int old_target_id = weapon->target_id;
    int old_target_type = weapon->target_type;
    
    Agent_state* agent = NULL;
    if (weapon->locked_on_target)
    {   // target locked
        // check target still exists
        if (weapon->target_type == OBJECT_AGENT)
            agent = STATE::agent_list->get(weapon->target_id);
        if (agent == NULL
        || vec3_distance_squared(agent->get_center(), camera_position) > weapon->sight_range*weapon->sight_range)
            weapon->locked_on_target = false;
    }

    if (!weapon->locked_on_target)
    {   // no target found
        // look for target
        weapon->lock_target(position);
    }

    if (weapon->target_type != OBJECT_NONE)
    {   // target found
        // lock target
        weapon->locked_on_target = true;
        motion->en_route = false;

        // send target packet
        if (!was_on_target || old_target_id != weapon->target_id || old_target_type != weapon->target_type)
            weapon->broadcast_target_choice();
    }
    
    if (weapon->locked_on_target)
    {   // target is locked
        // face target
        if (agent != NULL) // TODO -- multiple target
        {
            if (vec3_length(weapon->target_direction))
            {
                float theta,phi;
                vec3_to_angles(weapon->target_direction, &theta, &phi);
                physics->set_angles(vec3_init(theta, phi, 0));
            }
            if (weapon->can_fire())
                weapon->fire_on_target(camera_position);
        }
    }
    else if (motion->en_route)
    {   // face destination
        float theta, phi;
        vec3_to_angles(motion->target_direction, &theta, &phi);
        physics->set_angles(vec3_init(theta, phi, 0));
    }

    if (!motion->en_route && !weapon->locked_on_target)
    {   // no destination, no target
        // choose destination
        Vec3 destination;
        const int walk_len = MONSTER_BOX_WALK_RANGE;
        int dx = randrange(0,walk_len) - walk_len/2;
        int dy = randrange(0,walk_len) - walk_len/2;
        destination = vec3_add(position, vec3_init(((float)dx)+randf(), ((float)dy)+randf(),0));
        // clamp
        if (destination.x < 0) destination.x = 0;
        if (destination.x >= map_dim.x) destination.x = map_dim.x -1;
        if (destination.y < 0) destination.y = 0;
        if (destination.y >= map_dim.y) destination.y = map_dim.y -1;
        destination.z = (float)t_map::get_highest_open_block(destination.x,destination.y);
        if (destination.z < 0) destination.z = 0;

        motion->destination = destination;

        motion->en_route = true;
        motion->at_destination = false;
        
        Vec3 direction = vec3_sub(motion->destination, position);
        float len = vec3_length(direction);
        motion->ticks_to_destination = (int)ceil(len/motion->speed);
        if (len)
        {
            direction.z = 0;
            normalize_vector(&direction);
            motion->target_direction = direction;
        }
        motion->broadcast_destination();
    }

    if (!motion->at_destination)
    {   // check if at destination
        float dist = vec3_distance_squared(motion->destination, position);
        if (dist < 1.0f)    // TODO Margin
        {
            motion->en_route = false;
            motion->at_destination = true;
            physics->set_momentum(vec3_init(0,0,0));
        }
    }

    if (motion->en_route)
    {   // move towards destination
        motion->move_on_surface();

        // face in direction of movement
        float theta, phi;
        vec3_to_angles(motion->target_direction, &theta, &phi);
        physics->set_angles(vec3_init(theta, phi, 0));
    }

    //if (physics->changed)
        //object->broadcastState(); // send state packet if state changed
    //else if (this->canSendState())
        //object->broadcastState(); // send state packet every N ticks
}
#endif

#if DC_CLIENT
void client_tick_mob_robot_box(Object* object)
{
    return;
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* weapon = (WeaponTargetingComponent*)object->get_component(COMPONENT_WEAPON_TARGETING);

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)object->get_component(COMPONENT_MOTION_TARGETING);

    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    
    if (weapon->locked_on_target)
    {   // target locked
        if (weapon->target_type != OBJECT_AGENT) return;    // TODO -- more objects
        Agent_state* agent = ClientState::agent_list->get(weapon->target_id);
        if (agent == NULL) return;
        Vec3 agent_position = agent->get_center();

        using Components::VoxelModelComponent;
        VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component(COMPONENT_VOXEL_MODEL);
        
        // face target
        Vec3 position = vox->get_center();
        Vec3 direction = vec3_sub(agent_position, position);
        if (vec3_length(direction))
        {
            float theta,phi;
            normalize_vector(&direction);
            weapon->target_direction = direction;
            vec3_to_angles(direction, &theta, &phi);
            physics->set_angles(vec3_init(theta, phi, 0));
        }
        return; // do nothing else
    }

    if (!motion->at_destination)
    {   // check if at destination
        float dist = vec3_distance_squared(motion->destination, physics->get_position());
        if (dist < 1.0f)    // TODO Margin
        {   // at destination, stop
            motion->en_route = false;
            motion->at_destination = true;
            physics->set_momentum(vec3_init(0,0,0));
        }
    }

    if (motion->en_route)
    {   // move towards destination
        motion->move_on_surface();

        // face in direction of movement
        float theta, phi;
        vec3_to_angles(motion->target_direction, &theta, &phi);
        physics->set_angles(vec3_init(theta, phi, 0));
    }

}
#endif


void tick_mob_robot_box(Object* object)
{
    #if DC_SERVER
    server_tick_mob_robot_box(object);
    #endif
    #if DC_CLIENT
    client_tick_mob_robot_box(object);
    #endif
}

void update_mob_robot_box(Object* object)
{
    //return;
    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    //Vec3 angles = physics->get_angles();
    //vox->update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}


} // Objects

