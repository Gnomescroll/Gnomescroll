#include "blub.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/entities/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#if DC_SERVER
# include <entity/entities/mobs/state_machines.hpp>
#endif

namespace Entities
{

static void blub_state_router(class Entity*, EntityState state);

void load_mob_blub_data()
{
    EntityType type = ENTITY_MONSTER_BLUE_BLUB;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(PositionMomentum);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = MONSTER_BLUB_HEIGHT;

    auto mob = ADD_COMPONENT(SpriteMob);
    mob->mob.set_type("blue_blub");

    #if DC_CLIENT
    ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    auto health = ADD_COMPONENT(HitPoints);
    int health_amt = randrange(MONSTER_BLUB_HEALTH_MIN, MONSTER_BLUB_HEALTH_MAX);
    health->health = health_amt;
    health->health_max = health_amt;
    #endif

    auto dest = ADD_COMPONENT(DestinationTargeting);
    dest->sight_range = MONSTER_BLUB_MOTION_PROXIMITY_RADIUS;
    dest->destination_choice_x = MONSTER_BLUB_WALK_RANGE;
    dest->destination_choice_y = MONSTER_BLUB_WALK_RANGE;
    dest->speed = MONSTER_BLUB_WALK_SPEED;
    dest->max_z_diff = MONSTER_BLUB_MOTION_MAX_Z_DIFF;

    auto agent = ADD_COMPONENT(AgentTargeting);
    agent->sight_range = MONSTER_BLUB_MOTION_PROXIMITY_RADIUS;
    agent->speed = MONSTER_BLUB_CHASE_SPEED;
    agent->max_z_diff = MONSTER_BLUB_MOTION_MAX_Z_DIFF;
    agent->max_lock_ticks = MONSTER_BLUB_MAX_TARGET_LOCK_TICKS;
    agent->proximity_radius = MONSTER_BLUB_AGENT_STOP_PROXIMITY_RADIUS;
    agent->jump_force = MONSTER_BLUB_JUMP_FORCE;
    agent->set_jump_cooldowns(MONSTER_BLUB_JUMP_COOLDOWN_EN_ROUTE,
                              MONSTER_BLUB_JUMP_COOLDOWN_NEARBY);
    agent->attack_rate = (3 * MONSTER_BLUB_JUMP_COOLDOWN_NEARBY) / 4;
    agent->attack_damage = 2;
    agent->jump_near_player = true;

    auto waiting = ADD_COMPONENT(Waiting);
    waiting->wait_time = MONSTER_BLUB_IDLE_TIME;

    #if DC_SERVER
    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop->add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop->add_drop("synthesizer_coin", 3, 0.05f);
    item_drop->drop->set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop->add_drop_range("plasma_grenade", 1, 10, 0.8f);

    auto state = ADD_COMPONENT(StateMachine);
    state->state = STATE_WAITING;
    state->router = &blub_state_router;

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.0f;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = MONSTER_BLUB_ANIMATION_COLOR;
    anim->count = MONSTER_BLUB_ANIMATION_COUNT;
    anim->count_max = MONSTER_BLUB_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_BLUB_ANIMATION_SIZE;
    anim->force = MONSTER_BLUB_ANIMATION_FORCE;
    #endif
}

void ready_mob_blub(Entity* entity)
{
    auto mob = GET_COMPONENT_INTERFACE(SpriteMob, entity);
    mob->mob.associate_entity(entity->id, entity->type);
    mob->mob.init();

    #if DC_SERVER
    entity->broadcastCreate();
    #endif
}

void die_mob_blub(Entity* entity)
{
    #if DC_SERVER
    // drop item
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, entity);
    IF_ASSERT(item_drop == NULL) return;
    item_drop->drop_item();

    // notify clients
    entity->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto mob = GET_COMPONENT_INTERFACE(SpriteMob, entity);
    auto anim = GET_COMPONENT_INTERFACE(Animation, entity);
    anim->explode_random(mob->mob.get_center());
    #endif
}

#if DC_SERVER
static void blub_state_router(class Entity* entity, EntityState state)
{
    auto machine = GET_COMPONENT_INTERFACE(StateMachine, entity);

    switch (state)
    {
        case STATE_CHASE_AGENT:
            if (machine->state == STATE_WAITING)
                waiting_to_chase_agent(entity);
            else if (machine->state == STATE_IN_TRANSIT)
                in_transit_to_chase_agent(entity);
            break;

        case STATE_IN_TRANSIT:
            if (machine->state == STATE_WAITING)
                waiting_to_in_transit(entity);
            else if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_in_transit(entity);
            break;

        case STATE_WAITING:
            if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_waiting(entity);
            else if (machine->state == STATE_IN_TRANSIT)
                in_transit_to_waiting(entity);
            break;

        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }
}
#endif

#if DC_SERVER
void relax_blubs(Entity* entity)
{   // TODO -- this is inefficient -- we need to unpack all the physics
    // components into one array and work over that.
    const float relax_distance = 0.85f;
    const float relax_distance_sq = relax_distance * relax_distance;
    const float relax_force = 0.1f;
    int blub_count = 0;
    int blub_max = 0;
    class Entity* blubs = get_all(ENTITY_MONSTER_BLUE_BLUB, blub_max, blub_count);
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    Vec3 position = physics->get_position();
    Vec3 momentum = physics->get_momentum();
    for (int i=0, j=0; i<blub_max && j<blub_count; i++)
    {
        if (blubs[i].id == EntityList::null_id)
            continue;
        j++;
        Entity* blub = &blubs[i];
        if (blub->id == entity->id) continue;
        auto blub_physics = GET_COMPONENT_INTERFACE(Physics, blub);
        Vec3 p = blub_physics->get_position();
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

void tick_mob_blub(Entity* entity)
{
    #if DC_SERVER
    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, entity);
    if (limiter->allowed()) entity->broadcastState();

    auto machine = GET_COMPONENT_INTERFACE(StateMachine, entity);

    switch (machine->state)
    {
        case STATE_WAITING:
            waiting(entity);
            break;

        case STATE_IN_TRANSIT:
            in_transit(entity);
            break;

        case STATE_CHASE_AGENT:
            chase_agent(entity);
            break;

        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }

    if (machine->state != STATE_CHASE_AGENT)
    {   // aggro nearby agent
        auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
        Vec3 position = physics->get_position();

        auto target = GET_COMPONENT(AgentTargeting, entity);
        target->lock_target(position);

        if (target->target_type == ENTITY_AGENT)
            machine->router(entity, STATE_CHASE_AGENT);
    }

    // TODO -- call this rate limited
    relax_blubs(entity);

    #endif
}

void update_mob_blub(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    auto mob = GET_COMPONENT_INTERFACE(SpriteMob, entity);

    mob->mob.position = physics->get_position();
    physics->set_changed(false);  // reset changed state
    //printf("Slime position: "); vec3_print(physics->get_position());
}

} // Entities
