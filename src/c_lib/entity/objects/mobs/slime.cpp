#include "slime.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#if DC_SERVER
# include <entity/objects/mobs/state_machines.hpp>
#endif

namespace Entities
{

static void slime_state_router(class Entity*, EntityState state);

void load_mob_slime_data()
{
    EntityType type = OBJECT_MONSTER_SLIME;

    #if DC_SERVER
    const int n_components = 11;
    #endif
    #if DC_CLIENT
    const int n_components = 8;
    #endif

    entity_data->set_components(type, n_components);

    entity_data->attach_component(type, COMPONENT_POSITION_MOMENTUM);
    entity_data->attach_component(type, COMPONENT_DIMENSION);
    entity_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    entity_data->attach_component(type, COMPONENT_HIT_POINTS);
    entity_data->attach_component(type, COMPONENT_WAITING);
    entity_data->attach_component(type, COMPONENT_DESTINATION_TARGETING);
    entity_data->attach_component(type, COMPONENT_AGENT_TARGETING);

    #if DC_SERVER
    entity_data->attach_component(type, COMPONENT_STATE_MACHINE);
    entity_data->attach_component(type, COMPONENT_RATE_LIMIT);
    entity_data->attach_component(type, COMPONENT_ITEM_DROP);
    entity_data->attach_component(type, COMPONENT_KNOCKBACK);
    #endif

    #if DC_CLIENT
    entity_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_mob_slime_properties(Entity* object)
{
    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = MONSTER_SLIME_HEIGHT;

    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::slime;
    vox->init_hitscan = MONSTER_SLIME_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_SLIME_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    #if DC_CLIENT
    add_component_to_object(object, COMPONENT_HIT_POINTS);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    int health_amt = randrange(MONSTER_SLIME_HEALTH_MIN, MONSTER_SLIME_HEALTH_MAX);
    health->health = health_amt;
    health->health_max = health_amt;
    #endif

    using Components::DestinationTargetingComponent;
    DestinationTargetingComponent* dest = (DestinationTargetingComponent*)add_component_to_object(object, COMPONENT_DESTINATION_TARGETING);
    dest->sight_range = MONSTER_SLIME_MOTION_PROXIMITY_RADIUS;
    dest->destination_choice_x = MONSTER_SLIME_WALK_RANGE;
    dest->destination_choice_y = MONSTER_SLIME_WALK_RANGE;
    dest->speed = MONSTER_SLIME_WALK_SPEED;
    dest->max_z_diff = MONSTER_SLIME_MOTION_MAX_Z_DIFF;

    using Components::AgentTargetingComponent;
    AgentTargetingComponent* agent = (AgentTargetingComponent*)add_component_to_object(object, COMPONENT_AGENT_TARGETING);
    agent->sight_range = MONSTER_SLIME_MOTION_PROXIMITY_RADIUS;
    agent->speed = MONSTER_SLIME_CHASE_SPEED;
    agent->max_z_diff = MONSTER_SLIME_MOTION_MAX_Z_DIFF;
    agent->max_lock_ticks = MONSTER_SLIME_MAX_TARGET_LOCK_TICKS;
    agent->proximity_radius = MONSTER_SLIME_AGENT_STOP_PROXIMITY_RADIUS;
    agent->jump_force = MONSTER_SLIME_JUMP_FORCE;
    agent->set_jump_cooldowns(MONSTER_SLIME_JUMP_COOLDOWN_EN_ROUTE,
                              MONSTER_SLIME_JUMP_COOLDOWN_NEARBY);
    agent->attack_rate = (3 * MONSTER_SLIME_JUMP_COOLDOWN_NEARBY) / 4;
    agent->attack_damage = 2;

    using Components::WaitingComponent;
    WaitingComponent* waiting = (WaitingComponent*)add_component_to_object(object, COMPONENT_WAITING);
    waiting->wait_time = MONSTER_SLIME_IDLE_TIME;

    #if DC_SERVER
    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)add_component_to_object(object, COMPONENT_RATE_LIMIT);
    limiter->limit = MOB_BROADCAST_RATE;

    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)add_component_to_object(object, COMPONENT_ITEM_DROP);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop.add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop.add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop.add_drop("synthesizer_coin", 3, 0.05f);
    item_drop->drop.set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop.add_drop_range("plasma_grenade", 1, 10, 0.8f);

    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)add_component_to_object(object, COMPONENT_STATE_MACHINE);
    state->state = STATE_WAITING;
    state->router = &slime_state_router;

    using Components::KnockbackComponent;
    KnockbackComponent* knockback = (KnockbackComponent*)add_component_to_object(object, COMPONENT_KNOCKBACK);
    knockback->weight = 1.0f;
    #endif

    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->color = MONSTER_SLIME_ANIMATION_COLOR;
    anim->count = MONSTER_SLIME_ANIMATION_COUNT;
    anim->count_max = MONSTER_SLIME_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_SLIME_ANIMATION_SIZE;
    anim->force = MONSTER_SLIME_ANIMATION_FORCE;
    #endif

    object->tick = &tick_mob_slime;
    object->update = &update_mob_slime;

    object->create = create_packet_momentum_angles_health;
    object->state = state_packet_momentum_angles;
}

Entity* create_mob_slime()
{
    EntityType type = OBJECT_MONSTER_SLIME;
    Entity* obj = entity_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_slime_properties(obj);
    return obj;
}

void ready_mob_slime(Entity* object)
{
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

void die_mob_slime(Entity* object)
{
    #if DC_SERVER
    // drop item
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)object->get_component_interface(COMPONENT_INTERFACE_ITEM_DROP);
    IF_ASSERT(item_drop == NULL) return;
    item_drop->drop_item();

    // notify clients
    object->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    IF_ASSERT(vox->vox == NULL) return;
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)object->get_component_interface(COMPONENT_INTERFACE_ANIMATION);
    anim->explode_random(vox->get_center());
    #endif
}

#if DC_SERVER
static void slime_state_router(class Entity* object, EntityState state)
{
    using Components::StateMachineComponent;
    StateMachineComponent* machine = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);

    switch (state)
    {
        case STATE_CHASE_AGENT:
            if (machine->state == STATE_WAITING)
                waiting_to_chase_agent(object);
            //else if (machine->state == STATE_IN_TRANSIT)
                //in_transit_to_chase_agent(object);
            break;

        //case STATE_IN_TRANSIT:
            //if (machine->state == STATE_WAITING)
                //waiting_to_in_transit(object);
            //else if (machine->state == STATE_CHASE_AGENT)
                //chase_agent_to_in_transit(object);
            //break;

        case STATE_WAITING:
            if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_waiting(object);
            //else if (machine->state == STATE_IN_TRANSIT)
                //in_transit_to_waiting(object);
            break;

        case STATE_IN_TRANSIT:
        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }
}
#endif

#if DC_SERVER
void relax_slimes(Entity* object)
{   // TODO -- this is inefficient -- we need to unpack all the physics
    // components into one array and work over that.
    const float relax_distance = 0.85f;
    const float relax_distance_sq = relax_distance * relax_distance;
    const float relax_force = 0.1f;
    int slime_count = 0;
    char* slimes_used = NULL;
    class Entity** slimes = get_all(OBJECT_MONSTER_SLIME, slimes_used, slime_count);
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    Vec3 momentum = physics->get_momentum();
    for (int i=0; i<slime_count; i++)
    {
        if (!slimes_used[i]) continue;
        Entity* slime = slimes[i];
        if (slime->id == object->id) continue;
        PhysicsComponent* slime_physics = (PhysicsComponent*)slime->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        Vec3 p = slime_physics->get_position();
        float dist_sq = vec3_distance_squared(position, p);
        if (dist_sq > relax_distance_sq)
            continue;
        if (dist_sq == 0)
        {
            Vec3 push = vec3_init((2*randf()-1), (2*randf()-1), 0.0f);
            push = vec3_scalar_mult(push, relax_force);
            momentum = vec3_add(momentum, push);
            continue;
        }
        Vec3 push = vec3_sub(position, p);
        push = vec3_normalize(push);
        push = vec3_scalar_mult(push, relax_force);
        momentum = vec3_add(momentum, push);
    }
    physics->set_momentum(momentum);
}
#endif

void tick_mob_slime(Entity* object)
{
    #if DC_SERVER
    // TODO -- jump if near player

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)object->get_component_interface(COMPONENT_INTERFACE_RATE_LIMIT);
    if (limiter->allowed()) object->broadcastState();

    using Components::StateMachineComponent;
    StateMachineComponent* machine = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);

    switch (machine->state)
    {
        case STATE_WAITING:
            //waiting_for_agent(object);
            break;

        //case STATE_IN_TRANSIT:
            //in_transit(object);
            //break;

        case STATE_CHASE_AGENT:
            chase_agent(object);
            break;

        case STATE_IN_TRANSIT:
        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }

    if (machine->state != STATE_CHASE_AGENT)
    {   // aggro nearby agent
        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)
            object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        Vec3 position = physics->get_position();

        using Components::AgentTargetingComponent;
        AgentTargetingComponent* target = (AgentTargetingComponent*)object->get_component(COMPONENT_AGENT_TARGETING);
        target->lock_target(position);

        if (target->target_type == OBJECT_AGENT)
            machine->router(object, STATE_CHASE_AGENT);
    }

    // TODO -- call this rate limited
    relax_slimes(object);

    #endif
}

void update_mob_slime(Entity* object)
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
