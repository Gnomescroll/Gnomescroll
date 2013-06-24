#include "blub.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
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
    dims->height = 1.0f;

    auto mob = ADD_COMPONENT(SpriteMob);
    mob->mob.set_type("blue_blub");

    #if DC_CLIENT
    ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    auto health = ADD_COMPONENT(HitPoints);
    health->health = 50;
    health->health_max = 50;
    #endif

    auto dest = ADD_COMPONENT(DestinationTargeting);
    dest->sight_range = 15.0f;
    dest->destination_choice_x = 20;
    dest->destination_choice_y = 20;
    dest->speed = 0.1f;
    dest->max_z_diff = 3;

    auto agent = ADD_COMPONENT(AgentTargeting);
    agent->sight_range = 15.0f;
    agent->speed = 0.12f;
    agent->max_z_diff = 3;
    agent->max_lock_ticks = ONE_SECOND * 10;
    agent->proximity_radius = 2.1f;
    agent->jump_force = 0.38f;
    agent->set_jump_cooldowns((3 * ONE_SECOND) / 2, ONE_SECOND * 2);
    agent->attack_rate = (3 * ONE_SECOND) / 2;
    agent->attack_damage = 2;
    agent->jump_near_player = true;

    auto waiting = ADD_COMPONENT(Waiting);
    waiting->wait_time = ONE_SECOND * 3;

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
    anim->color = Color(21, 21, 77);
    anim->count = 25;
    anim->count_max = 40;
    anim->size = 0.2f;
    anim->force = 1.0f;
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
