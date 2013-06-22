#pragma once

#include <entity/components/physics.hpp>
#include <entity/components/targeting/destination_targeting.hpp>
#include <entity/components/targeting/agent_targeting.hpp>
#include <entity/components/state_machine.hpp>
#include <entity/components/waiting.hpp>
#include <entity/macros.hpp>

namespace Entities
{

static void go_to_next_destination(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    Vec3 position = physics->get_position();

    // choose new position destination
    auto dest = GET_COMPONENT(DestinationTargeting, entity);

    dest->choose_destination();

    // face the target
    dest->orient_to_target(position);
    Vec3 angles = physics->get_angles();
    angles.x = vec3_to_theta(dest->target_direction);  // only rotate in x
    physics->set_angles(angles);
}

static void waiting_to_in_transit(Entity* entity)
{
    go_to_next_destination(entity);
    auto state = GET_COMPONENT_INTERFACE(StateMachine, entity);
    state->state = STATE_IN_TRANSIT;
}

static void waiting_to_chase_agent(Entity* entity)
{   // assumes target already locked
    auto state = GET_COMPONENT_INTERFACE(StateMachine, entity);
    state->state = STATE_CHASE_AGENT;
}

static void in_transit_to_waiting(Entity* entity)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, entity);
    waiting->tick = 0;

    auto state = GET_COMPONENT_INTERFACE(StateMachine, entity);
    state->state = STATE_WAITING;
}

static void in_transit_to_chase_agent(Entity* entity)
{   // assumes target already locked
    auto state = GET_COMPONENT_INTERFACE(StateMachine, entity);
    state->state = STATE_CHASE_AGENT;
}

static void chase_agent_to_waiting(Entity* entity)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, entity);
    waiting->tick = 0;

    auto state = GET_COMPONENT_INTERFACE(StateMachine, entity);
    state->state = STATE_WAITING;
}

static void chase_agent_to_in_transit(Entity* entity)
{   // unused
    GS_ASSERT(false);
}

static void waiting(Entity* entity)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, entity);
    waiting->tick++;
    if (waiting->ready())
        waiting_to_in_transit(entity);
}

static void in_transit(Entity* entity)
{
    auto dest_target = GET_COMPONENT(DestinationTargeting, entity);

    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    // move towards target
    dest_target->move_on_surface();
    if (!physics->get_changed())
    {   // failed to move
        in_transit_to_waiting(entity);
    }
    else
    {   // check at destination
        if (dest_target->check_at_destination())
        {
            if (dest_target->path_finished())
                in_transit_to_waiting(entity);
            else
                go_to_next_destination(entity);
        }
        else
            dest_target->orient_to_target(physics->get_position());
    }
}

static void chase_agent(Entity* entity)
{   // TODO -- write a method that switches to STATE_WAITING under certain conditions
    // for slimes, this would be if we travelled too far from our original spawn location
    auto target = GET_COMPONENT(AgentTargeting, entity);

    // check target
    target->check_target_alive();
    if (target->target_type == NULL_ENTITY_TYPE)
    {
        chase_agent_to_waiting(entity);
        return;
    }

    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    Vec3 position = physics->get_position();
    // face the target
    target->orient_to_target(position);
    Vec3 angles = physics->get_angles();
    angles.x = vec3_to_theta(target->target_direction); // only rotate in x
    physics->set_angles(angles);

    // move towards target
    target->move_on_surface();
}

}   // Entities
