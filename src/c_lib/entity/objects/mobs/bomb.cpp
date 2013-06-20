#include "bomb.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#if DC_SERVER
# include <entity/components/explosion.hpp>
# include <entity/objects/mobs/state_machines.hpp>
#endif

namespace Entities
{

static void bomb_state_router(class Entity*, EntityState state);

void load_mob_bomb_data()
{
    EntityType type = ENTITY_MONSTER_BOMB;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(PositionMomentum);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = MONSTER_BOMB_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::robot_bomb;
    vox->init_hitscan = MONSTER_BOMB_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_BOMB_INIT_WITH_DRAW;

    #if DC_CLIENT
    ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    auto health = ADD_COMPONENT(HitPoints);
    int health_amt = randrange(MONSTER_BOMB_HEALTH_MIN, MONSTER_BOMB_HEALTH_MAX);
    health->health = health_amt;
    health->health_max = health_amt;
    #endif

    auto dest = ADD_COMPONENT(DestinationTargeting);
    dest->sight_range = MONSTER_BOMB_MOTION_PROXIMITY_RADIUS;
    dest->destination_choice_x = MONSTER_BOMB_WALK_RANGE;
    dest->destination_choice_y = MONSTER_BOMB_WALK_RANGE;
    dest->speed = MONSTER_BOMB_WALK_SPEED;
    dest->max_z_diff = MONSTER_BOMB_MOTION_MAX_Z_DIFF;

    auto agent = ADD_COMPONENT(AgentTargeting);
    agent->sight_range = MONSTER_BOMB_MOTION_PROXIMITY_RADIUS;
    agent->speed = MONSTER_BOMB_CHASE_SPEED;
    agent->max_z_diff = MONSTER_BOMB_MOTION_MAX_Z_DIFF;
    agent->max_lock_ticks = MONSTER_BOMB_MAX_TARGET_LOCK_TICKS;

    auto waiting = ADD_COMPONENT(Waiting);
    waiting->wait_time = MONSTER_BOMB_IDLE_TIME;

    #if DC_SERVER
    auto explode = ADD_COMPONENT(Explosion);
    explode->radius = MONSTER_BOMB_EXPLOSION_RADIUS;
    explode->proximity_radius = MONSTER_BOMB_EXPLOSION_PROXIMITY_RADIUS;
    explode->damage = MONSTER_BOMB_EXPLOSION_DAMAGE;
    explode->block_destruction_radius = MONSTER_BOMB_BLOCK_DESTRUCTION_RADIUS;
    explode->block_damage = MONSTER_BOMB_BLOCK_DAMAGE;
    explode->terrain_modification_action = TMA_MONSTER_BOMB;
    explode->delay = MOB_BROADCAST_RATE;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop.add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop.add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop.add_drop("synthesizer_coin", 3, 0.05f);

    item_drop->drop.set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop.add_drop_range("plasma_grenade", 1, 10, 0.8f);

    auto state = ADD_COMPONENT(StateMachine);
    state->state = STATE_WAITING;
    state->router = &bomb_state_router;

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.0f;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = MONSTER_BOMB_ANIMATION_COLOR;
    anim->count = MONSTER_BOMB_ANIMATION_COUNT;
    anim->count_max = MONSTER_BOMB_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_BOMB_ANIMATION_SIZE;
    anim->force = MONSTER_BOMB_ANIMATION_FORCE;
    #endif
}

void ready_mob_bomb(Entity* object)
{

    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_mob_bomb(Entity* object)
{
    #if DC_SERVER
    // drop item
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, object);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    // explosion damage
    auto explode = GET_COMPONENT_INTERFACE(Explosion, object);
    explode->explode();
    explode->damage_blocks();

    // notify clients
    object->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    if (vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, object);
        anim->explode_random(vox->get_center());
    }
    #endif
}

#if DC_SERVER
static void bomb_state_router(class Entity* object, EntityState state)
{
    auto machine = GET_COMPONENT_INTERFACE(StateMachine, object);

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

        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }
}
#endif

void tick_mob_bomb(Entity* object)
{
    #if DC_SERVER
     //die if near agent
    auto explode = GET_COMPONENT_INTERFACE(Explosion, object);
    explode->proximity_check();

    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, object);
    if (limiter->allowed()) object->broadcastState();

    auto machine = GET_COMPONENT_INTERFACE(StateMachine, object);

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

        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }

    if (machine->state != STATE_CHASE_AGENT)
    {   // aggro nearby agent
        auto physics = GET_COMPONENT_INTERFACE(Physics, object);
        Vec3 position = physics->get_position();

        auto target = GET_COMPONENT(AgentTargeting, object);
        target->lock_target(position);

        if (target->target_type == ENTITY_AGENT)
            machine->router(object, STATE_CHASE_AGENT);
    }
    #endif
}

void update_mob_bomb(Entity* object)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
