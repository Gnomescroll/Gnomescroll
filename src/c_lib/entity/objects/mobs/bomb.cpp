#include "bomb.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#if DC_SERVER
#include <entity/components/explosion.hpp>
#endif

namespace Objects
{
    
static void bomb_state_router(class Object*, EntityState state);

void load_mob_bomb_data()
{
    ObjectType type = OBJECT_MONSTER_BOMB;
    
    #if DC_SERVER
    const int n_components = 11;
    #endif
    #if DC_CLIENT
    const int n_components = 9;
    #endif
    
    object_data->set_components(type, n_components);
    
    object_data->attach_component(type, COMPONENT_POSITION_MOMENTUM_CHANGED);
    object_data->attach_component(type, COMPONENT_DIMENSION);
    object_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    object_data->attach_component(type, COMPONENT_HIT_POINTS);
    object_data->attach_component(type, COMPONENT_STATE_MACHINE);
    object_data->attach_component(type, COMPONENT_WAITING);
    object_data->attach_component(type, COMPONENT_DESTINATION_TARGETING);
    object_data->attach_component(type, COMPONENT_AGENT_TARGETING);

    #if DC_SERVER
    object_data->attach_component(type, COMPONENT_EXPLOSION);
    object_data->attach_component(type, COMPONENT_RATE_LIMIT);
    object_data->attach_component(type, COMPONENT_ITEM_DROP);
    #endif

    #if DC_CLIENT
    object_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_mob_bomb_properties(Object* object)
{
    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM_CHANGED);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = MONSTER_BOMB_HEIGHT;

    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::robot_bomb;
    vox->init_hitscan = MONSTER_BOMB_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_BOMB_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    #if DC_CLIENT
    add_component_to_object(object, COMPONENT_HIT_POINTS);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    int health_amt = randrange(MONSTER_BOMB_HEALTH_MIN, MONSTER_BOMB_HEALTH_MAX);
    health->health = health_amt;
    health->max_health = health_amt;
    #endif

    using Components::DestinationTargetingComponent;
    DestinationTargetingComponent* dest = (DestinationTargetingComponent*)add_component_to_object(object, COMPONENT_DESTINATION_TARGETING);
    dest->sight_range = MONSTER_BOMB_MOTION_PROXIMITY_RADIUS;
    dest->destination_choice_x = MONSTER_BOMB_WALK_RANGE;
    dest->destination_choice_y = MONSTER_BOMB_WALK_RANGE;
    dest->speed = MONSTER_BOMB_WALK_SPEED;
    dest->max_z_diff = MONSTER_BOMB_MOTION_MAX_Z_DIFF;
    
    using Components::AgentTargetingComponent;
    AgentTargetingComponent* agent = (AgentTargetingComponent*)add_component_to_object(object, COMPONENT_AGENT_TARGETING);
    agent->sight_range = MONSTER_BOMB_MOTION_PROXIMITY_RADIUS;
    agent->speed = MONSTER_BOMB_CHASE_SPEED;
    agent->max_z_diff = MONSTER_BOMB_MOTION_MAX_Z_DIFF;
    agent->max_lock_ticks = MONSTER_BOMB_MAX_TARGET_LOCK_TICKS;

    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)add_component_to_object(object, COMPONENT_STATE_MACHINE);
    state->state = STATE_WAITING;
    state->router = &bomb_state_router;
    
    using Components::WaitingComponent;
    WaitingComponent* waiting = (WaitingComponent*)add_component_to_object(object, COMPONENT_WAITING);
    waiting->wait_time = MONSTER_BOMB_IDLE_TIME;

    #if DC_SERVER
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)add_component_to_object(object, COMPONENT_EXPLOSION);
    explode->radius = MONSTER_BOMB_EXPLOSION_RADIUS;
    explode->proximity_radius = MONSTER_BOMB_EXPLOSION_PROXIMITY_RADIUS;
    explode->damage = MONSTER_BOMB_EXPLOSION_DAMAGE;
    explode->block_destruction_radius = MONSTER_BOMB_BLOCK_DESTRUCTION_RADIUS;
    explode->block_damage = MONSTER_BOMB_BLOCK_DAMAGE;
    explode->terrain_modification_action = t_map::TMA_MONSTER_BOMB;

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)add_component_to_object(object, COMPONENT_RATE_LIMIT);
    limiter->limit = MONSTER_BOMB_BROADCAST_RATE;

    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)add_component_to_object(object, COMPONENT_ITEM_DROP);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop.add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop.add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop.add_drop("synthesizer_coin", 3, 0.05f);
    
    item_drop->drop.set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop.add_drop_range("plasma_grenade", 1, 10, 0.8f);
    #endif

    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->color = MONSTER_BOMB_ANIMATION_COLOR;
    anim->count = MONSTER_BOMB_ANIMATION_COUNT;
    anim->count_max = MONSTER_BOMB_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_BOMB_ANIMATION_SIZE;
    anim->force = MONSTER_BOMB_ANIMATION_FORCE;
    #endif
    
    object->tick = &tick_mob_bomb;
    object->update = &update_mob_bomb;

    object->create = create_packet_momentum_angles_health;
    object->state = state_packet_momentum_angles;
}

Object* create_mob_bomb()
{
    ObjectType type = OBJECT_MONSTER_BOMB;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_bomb_properties(obj);
    return obj;
}

void ready_mob_bomb(Object* object)
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

void die_mob_bomb(Object* object)
{
    #if DC_SERVER
    // drop item
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)object->get_component_interface(COMPONENT_INTERFACE_ITEM_DROP);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();
    
    // explosion damage
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    explode->explode();
    explode->damage_blocks();
    object->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox->vox != NULL)
    {
        using Components::AnimationComponent;
        AnimationComponent* anim = (AnimationComponent*)object->get_component_interface(COMPONENT_INTERFACE_ANIMATION);
        anim->explode_random(vox->get_center());
    }
    #endif
}

#if DC_SERVER
static bool pack_object_in_transit(class object_in_transit_StoC* msg, class Object* object, class Components::DestinationTargetingComponent* dest)
{
    if (dest == NULL)
        dest = (Components::DestinationTargetingComponent*)
            object->get_component(COMPONENT_DESTINATION_TARGETING);

    GS_ASSERT(dest != NULL);
    if (dest == NULL) return false;

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)
        object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    GS_ASSERT(physics != NULL);
    if (physics == NULL) return false;

    msg->type = object->type;
    msg->id = object->id;
    Vec3 destination = dest->get_destination();
    ASSERT_BOXED_POSITION(destination);
    msg->destination = destination;
    //msg->dir = dest->target_direction;
    int ticks = dest->get_ticks_to_destination(physics->get_position());
    msg->ticks_to_destination = ticks;
    
    return true;
}

void broadcast_object_in_transit(class Object* object, class Components::DestinationTargetingComponent* dest)
{
    object_in_transit_StoC msg;
    if (!pack_object_in_transit(&msg, object, dest)) return;
    msg.broadcast();
}

void send_object_in_transit(int client_id, class Object* object, class Components::DestinationTargetingComponent* dest)
{
    object_in_transit_StoC msg;
    if (!pack_object_in_transit(&msg, object, dest)) return;
    msg.sendToClient(client_id);
}

static bool pack_object_chase_agent(class object_chase_agent_StoC* msg, class Object* object, class Components::AgentTargetingComponent* target)
{
    if (target == NULL)
        target = (Components::AgentTargetingComponent*)
            object->get_component(COMPONENT_AGENT_TARGETING);

    if (target == NULL) return false;

    msg->type = object->type;
    msg->id = object->id;
    msg->target_id = target->target_id;
    
    return true;
}

void broadcast_object_chase_agent(class Object* object, class Components::AgentTargetingComponent* target)
{
    object_chase_agent_StoC msg;
    if (!pack_object_chase_agent(&msg, object, target)) return;
    msg.broadcast();
}

void send_object_chase_agent(int client_id, class Object* object, class Components::AgentTargetingComponent* target)
{
    object_chase_agent_StoC msg;
    if (!pack_object_chase_agent(&msg, object, target)) return;
    msg.sendToClient(client_id);
}

static bool pack_object_begin_wait(class object_begin_waiting_StoC* msg, class Object* object)
{
    msg->type = object->type;
    msg->id = object->id;
    return true;
}

void broadcast_object_begin_wait(class Object* object)
{
    object_begin_waiting_StoC msg;
    if (!pack_object_begin_wait(&msg, object)) return;
    msg.broadcast();
}

void send_object_begin_wait(int client_id, class Object* object)
{
    object_begin_waiting_StoC msg;
    if (!pack_object_begin_wait(&msg, object)) return;
    msg.sendToClient(client_id);
}

inline void send_mob_bomb_state_machine_to_client(int client_id, class Object* object)
{
    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);

    if (state->state == STATE_WAITING)
        send_object_begin_wait(client_id, object);
    //else
    //if (state->state == STATE_IN_TRANSIT)
        //send_object_in_transit(client_id, object, NULL);
    //else
    //if (state->state == STATE_CHASE_AGENT)
        //send_object_chase_agent(client_id, object, NULL);
}
#endif

static void waiting_to_in_transit(class Object* object)
{
    #if DC_SERVER
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();

    // choose new position destination
    using Components::DestinationTargetingComponent;
    DestinationTargetingComponent* dest = (DestinationTargetingComponent*)
        object->get_component(COMPONENT_DESTINATION_TARGETING);
        
    dest->choose_destination();

    // face the target
    dest->orient_to_target(position);    
    Vec3 angles = physics->get_angles();
    angles.x = vec3_to_theta(dest->target_direction); // only rotate in x
    physics->set_angles(angles);

    broadcast_object_in_transit(object, dest);
    #endif

    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_IN_TRANSIT;
}

static void waiting_to_chase_agent(class Object* object)
{   // assumes target already locked
    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_CHASE_AGENT;

    #if DC_SERVER
    broadcast_object_chase_agent(object, NULL);
    #endif
}

static void in_transit_to_waiting(class Object* object)
{
    using Components::WaitingComponent;
    WaitingComponent* wait = (WaitingComponent*)object->get_component_interface(COMPONENT_INTERFACE_WAITING);
    wait->tick = 0;

    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_WAITING;
    
    #if DC_SERVER
    broadcast_object_begin_wait(object);
    #endif
}

static void in_transit_to_chase_agent(class Object* object)
{   // assumes target already locked
    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_CHASE_AGENT;

    #if DC_SERVER
    broadcast_object_chase_agent(object, NULL);
    #endif
}

static void chase_agent_to_waiting(class Object* object)
{
    using Components::WaitingComponent;
    WaitingComponent* wait = (WaitingComponent*)object->get_component_interface(COMPONENT_INTERFACE_WAITING);
    wait->tick = 0;
    
    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_WAITING;

    #if DC_SERVER
    broadcast_object_begin_wait(object);
    #endif
}

static void chase_agent_to_in_transit(class Object* object)
{   // unused
    GS_ASSERT(false);
}

// ticks to do while in a state

static void waiting(class Object* object)
{
    #if DC_SERVER
    using Components::WaitingComponent;
    WaitingComponent* wait = (WaitingComponent*)object->get_component_interface(COMPONENT_INTERFACE_WAITING);
    wait->tick++;
    if (wait->ready())
        waiting_to_in_transit(object);
    #endif
}

static void in_transit(class Object* object)
{
    using Components::DestinationTargetingComponent;
    DestinationTargetingComponent* dest_target = (DestinationTargetingComponent*)object->get_component(COMPONENT_DESTINATION_TARGETING);

    #if DC_CLIENT
    if (!dest_target->at_destination)
    {   // DONT UNCOMMENT THIS: BAD: MYSTERY ERROR:
        //dest_target->orient_to_target(position);    
        //Vec3 angles = physics->get_angles();
        //angles.x = vec3_to_theta(dest_target->target_direction); // only rotate in x
        //physics->set_angles(angles);

        dest_target->move_on_surface();
        dest_target->check_at_destination();
    }
    #endif
    
    #if DC_SERVER
    // move towards target
    if (!dest_target->move_on_surface())
        in_transit_to_waiting(object);  // failed to move
    else
    {   // check at destination
        if (dest_target->check_at_destination())
            in_transit_to_waiting(object);
        //else
        //{
            //using Components::PhysicsComponent;
            //PhysicsComponent* physics = (PhysicsComponent*)
                //object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            //dest_target->orient_to_target(physics->get_position());
        //}
    }
    #endif
}

static void chase_agent(class Object* object)
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)
        object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();

    using Components::AgentTargetingComponent;
    AgentTargetingComponent* target = (AgentTargetingComponent*)object->get_component(COMPONENT_AGENT_TARGETING);
    
    // check target
    target->check_target_alive();
    if (target->target_type == OBJECT_NONE)
    {
        chase_agent_to_waiting(object);
        return;
    }

    // face the target
    target->orient_to_target(position);    
    Vec3 angles = physics->get_angles();
    angles.x = vec3_to_theta(target->target_direction); // only rotate in x
    physics->set_angles(angles);

    // move towards target
    target->move_on_surface();
}

static void bomb_state_router(class Object* object, EntityState state)
{
    using Components::StateMachineComponent;
    StateMachineComponent* machine = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);

    switch (state)
    {
        case STATE_CHASE_AGENT:
            if (machine->state == STATE_WAITING)
                waiting_to_chase_agent(object);
            else if (machine->state == STATE_IN_TRANSIT)
                in_transit_to_chase_agent(object);
            break;
        
        case STATE_IN_TRANSIT:
            if (machine->state == STATE_WAITING)
                waiting_to_in_transit(object);
            else if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_in_transit(object);
            break;
        
        case STATE_WAITING:
            if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_waiting(object);
            else if (machine->state == STATE_IN_TRANSIT)
                in_transit_to_waiting(object);
            break;
            
        default:
            GS_ASSERT(false);
            break;
    }
}

void tick_mob_bomb(Object* object)
{
    #if DC_SERVER
     //die if near agent
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    explode->proximity_check();

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)object->get_component_interface(COMPONENT_INTERFACE_RATE_LIMIT);
    if (limiter->allowed())
        object->broadcastState();
    #endif

    using Components::StateMachineComponent;
    StateMachineComponent* machine = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);

    // increment state
    switch (machine->state)
    {
        case STATE_WAITING:
            waiting(object);
            break;

        case STATE_IN_TRANSIT:
            in_transit(object);
            break;

        case STATE_CHASE_AGENT:
            chase_agent(object);
            break;
            
        default:
            GS_ASSERT(false);
            break;
    }
    
    #if DC_SERVER
    // aggro nearby agent
    if (machine->state != STATE_CHASE_AGENT)
    {
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
    #endif
}

void update_mob_bomb(Object* object)
{
    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}

} // Objects
