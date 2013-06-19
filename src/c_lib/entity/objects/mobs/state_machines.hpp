#pragma once

#include <entity/components/physics.hpp>
#include <entity/components/targeting/destination_targeting.hpp>
#include <entity/components/targeting/agent_targeting.hpp>
#include <entity/components/state_machine.hpp>
#include <entity/components/waiting.hpp>
#include <entity/macros.hpp>

namespace Entities
{

static void go_to_next_destination(Entity* object)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    Vec3 position = physics->get_position();

    // choose new position destination
    auto dest = GET_COMPONENT(DestinationTargeting, object);

    dest->choose_destination();

    // face the target
    dest->orient_to_target(position);
    Vec3 angles = physics->get_angles();
    angles.x = vec3_to_theta(dest->target_direction);  // only rotate in x
    physics->set_angles(angles);
}

static void waiting_to_in_transit(Entity* object)
{
    go_to_next_destination(object);
    auto state = GET_COMPONENT_INTERFACE(StateMachine, object);
    state->state = STATE_IN_TRANSIT;
}

static void waiting_to_chase_agent(Entity* object)
{   // assumes target already locked
    auto state = GET_COMPONENT_INTERFACE(StateMachine, object);
    state->state = STATE_CHASE_AGENT;
}

static void in_transit_to_waiting(Entity* object)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, object);
    waiting->tick = 0;

    auto state = GET_COMPONENT_INTERFACE(StateMachine, object);
    state->state = STATE_WAITING;
}

static void in_transit_to_chase_agent(Entity* object)
{   // assumes target already locked
    auto state = GET_COMPONENT_INTERFACE(StateMachine, object);
    state->state = STATE_CHASE_AGENT;
}

static void chase_agent_to_waiting(Entity* object)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, object);
    waiting->tick = 0;

    auto state = GET_COMPONENT_INTERFACE(StateMachine, object);
    state->state = STATE_WAITING;
}

static void chase_agent_to_in_transit(Entity* object)
{   // unused
    GS_ASSERT(false);
}

static void waiting(Entity* object)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, object);
    waiting->tick++;
    if (waiting->ready())
        waiting_to_in_transit(object);
}

static void in_transit(Entity* object)
{
    auto dest_target = GET_COMPONENT(DestinationTargeting, object);

    auto physics = GET_COMPONENT_INTERFACE(Physics, object);

    // move towards target
    dest_target->move_on_surface();
    if (!physics->get_changed())
    {   // failed to move
        in_transit_to_waiting(object);
    }
    else
    {   // check at destination
        if (dest_target->check_at_destination())
        {
            if (dest_target->path_finished())
                in_transit_to_waiting(object);
            else
                go_to_next_destination(object);
        }
        else
            dest_target->orient_to_target(physics->get_position());
    }
}

static void chase_agent(Entity* object)
{   // TODO -- write a method that switches to STATE_WAITING under certain conditions
    // for slimes, this would be if we travelled too far from our original spawn location
    auto target = GET_COMPONENT(AgentTargeting, object);

    // check target
    target->check_target_alive();
    if (target->target_type == NULL_ENTITY_TYPE)
    {
        chase_agent_to_waiting(object);
        return;
    }

    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
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
