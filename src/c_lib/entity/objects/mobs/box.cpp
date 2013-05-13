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
#include <agent/_interface.hpp>

namespace Entities
{

void load_mob_robot_box_data()
{
    EntityType type = OBJECT_MONSTER_BOX;

    #if DC_SERVER
    const int n_components = 10;
    #endif
    #if DC_CLIENT
    const int n_components = 7;
    #endif

    entity_data->set_components(type, n_components);

    entity_data->attach_component(type, COMPONENT_POSITION_MOMENTUM);
    entity_data->attach_component(type, COMPONENT_DIMENSION);
    entity_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    entity_data->attach_component(type, COMPONENT_HIT_POINTS);
    entity_data->attach_component(type, COMPONENT_WEAPON_TARGETING);
    entity_data->attach_component(type, COMPONENT_MOTION_TARGETING);

    #if DC_SERVER
    entity_data->attach_component(type, COMPONENT_SPAWN_CHILD);
    entity_data->attach_component(type, COMPONENT_RATE_LIMIT);
    entity_data->attach_component(type, COMPONENT_ITEM_DROP);
    entity_data->attach_component(type, COMPONENT_KNOCKBACK);
    #endif
    #if DC_CLIENT
    entity_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_mob_robot_box_properties(Entity* object)
{
    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM);

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
    #if DC_CLIENT
    add_component_to_object(object, COMPONENT_HIT_POINTS);
    #endif
    #if DC_SERVER   // health will be set by packet initializer
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    int health_amt = randrange(MONSTER_BOX_HEALTH_MIN, MONSTER_BOX_HEALTH_MAX);
    health->health = health_amt;
    health->health_max = health_amt;
    #endif

    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* target = (WeaponTargetingComponent*)add_component_to_object(object, COMPONENT_WEAPON_TARGETING);
    target->target_acquisition_failure_rate = MONSTER_BOX_TARGET_ACQUISITION_FAILURE_RATE;
    target->fire_rate_limit = MONSTER_BOX_FIRE_RATE_LIMIT;
    target->uses_bias = MONSTER_BOX_USES_BIAS;
    target->accuracy_bias = MONSTER_BOX_ACCURACY_BIAS;
    target->sight_range = MONSTER_BOX_FIRING_SIGHT_RANGE;
    target->attack_at_random = MONSTER_BOX_ATTACK_AT_RANDOM;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = OBJECT_MONSTER_BOX;
    target->attacker_properties.block_damage = MONSTER_BOX_TERRAIN_DAMAGE;
    target->attacker_properties.agent_damage_min = MONSTER_BOX_AGENT_DAMAGE_MIN;
    target->attacker_properties.agent_damage_max = MONSTER_BOX_AGENT_DAMAGE_MAX;
    //target->attacker_properties.voxel_damage_radius = MONSTER_BOX_VOXEL_DAMAGE_RADIUS;
    target->attacker_properties.agent_protection_duration = MONSTER_BOX_AGENT_IMMUNITY_DURATION;
    target->attacker_properties.terrain_modification_action = TMA_MONSTER_BOX;
    target->fire_delay_max = MONSTER_BOX_FIRE_DELAY_MAX;

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)add_component_to_object(object, COMPONENT_MOTION_TARGETING);
    motion->speed = MONSTER_BOX_SPEED;
    motion->max_z_diff = MONSTER_BOX_MOTION_MAX_Z_DIFF;

    #if DC_SERVER
    add_component_to_object(object, COMPONENT_SPAWN_CHILD);

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)add_component_to_object(object, COMPONENT_RATE_LIMIT);
    limiter->limit = MOB_BROADCAST_RATE;

    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)add_component_to_object(object, COMPONENT_ITEM_DROP);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop.add_drop("synthesizer_coin", 1, 0.2f);
    item_drop->drop.add_drop("synthesizer_coin", 2, 0.05f);
    item_drop->drop.add_drop("synthesizer_coin", 3, 0.01f);

    item_drop->drop.set_max_drop_amounts("small_charge_pack", 1);
    item_drop->drop.add_drop("small_charge_pack", 1, 0.02f);

    using Components::KnockbackComponent;
    KnockbackComponent* knockback = (KnockbackComponent*)add_component_to_object(object, COMPONENT_KNOCKBACK);
    knockback->weight = 1.5f;
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

    object->create = create_packet_momentum_angles_health;
    object->state = state_packet_momentum_angles;
}

Entity* create_mob_robot_box()
{
    EntityType type = OBJECT_MONSTER_BOX;
    Entity* obj = entity_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_robot_box_properties(obj);
    return obj;
}

void ready_mob_robot_box(Entity* object)
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

void die_mob_robot_box(Entity* object)
{
    #if DC_SERVER
    // drop item
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)object->get_component_interface(COMPONENT_INTERFACE_ITEM_DROP);
    GS_ASSERT(item_drop != NULL);
    if (item_drop != NULL) item_drop->drop_item();

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
void server_tick_mob_robot_box(Entity* object)
{
    // must stay on ground -- apply terrain collision
    // wander randomly (TODO: network model with destinations)
    // TODO -- aggro component

    typedef Components::PositionMomentumPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM);
    const Vec3 position = physics->get_position();
    Vec3 camera_position = position;

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    camera_position.z += dims->get_camera_height();

    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* weapon = (WeaponTargetingComponent*)object->get_component(COMPONENT_WEAPON_TARGETING);
    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)object->get_component(COMPONENT_MOTION_TARGETING);

    Agents::Agent* agent = NULL;
    if (weapon->locked_on_target)
    {   // target locked
        // query agent
        if (weapon->target_type == OBJECT_AGENT)
            agent = Agents::get_agent((AgentID)weapon->target_id);
        // check target still exists
        if (!weapon->target_is_visible(camera_position))
            weapon->locked_on_target = false;
    }

    if (!weapon->locked_on_target)
    {   // no target found
        // look for target
        weapon->lock_target(camera_position);
        if (weapon->locked_on_target)
            agent = Agents::get_agent((AgentID)weapon->target_id);
    }

    if (weapon->target_type != OBJECT_NONE)
    {   // target found
        // lock target
        weapon->locked_on_target = true;
        motion->en_route = false;
    }

    if (weapon->locked_on_target)
    {   // target is locked
        // face target
        if (agent != NULL) // TODO -- multiple target types
        {
            if (vec3_length_squared(weapon->target_direction))
            {
                float theta,phi;
                vec3_to_angles(weapon->target_direction, &theta, &phi);
                physics->set_angles(vec3_init(theta, phi, 0));
            }
            weapon->tick();
            if (weapon->can_lock_part())
            {
                weapon->lock_target_part(camera_position);
                weapon->add_random_fire_delay();
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
        destination = vec3_add(position, vec3_init(float(dx)+randf(), float(dy)+randf(),0));
        // clamp
        destination.z = t_map::get_nearest_surface_block(destination.x, destination.y, destination.z);
        destination.z = GS_MAX(0, destination.z);

        Vec3 direction = vec3_sub(destination, position);
        float len = vec3_length(direction);

        motion->ticks_to_destination = int(ceilf(len/motion->speed));
        motion->destination = translate_position(destination);
        motion->en_route = true;
        motion->at_destination = false;

        if (len)
        {
            direction.z = 0;
            if (vec3_length_squared(direction))
                direction = vec3_normalize(direction);
            motion->target_direction = direction;
        }
    }

    if (!motion->at_destination)
    {   // check if at destination
        float dist = vec3_distance_squared(motion->destination, position);
        if (dist < 1.0f)    // TODO Margin
        {
            motion->en_route = false;
            motion->at_destination = true;
            physics->set_momentum(vec3_init(0));
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

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)object->get_component_interface(COMPONENT_INTERFACE_RATE_LIMIT);
    IF_ASSERT(limiter == NULL) return;
    if (limiter->allowed()) object->broadcastState();
}
#endif

#if DC_CLIENT
void client_tick_mob_robot_box(Entity* object)
{
}
#endif

void tick_mob_robot_box(Entity* object)
{
    #if DC_SERVER
    server_tick_mob_robot_box(object);
    #endif
    #if DC_CLIENT
    client_tick_mob_robot_box(object);
    #endif
}

void update_mob_robot_box(Entity* object)
{
    typedef Components::PositionMomentumPhysicsComponent PCP;
    using Components::VoxelModelComponent;

    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
