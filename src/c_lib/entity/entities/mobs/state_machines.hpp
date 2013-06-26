#pragma once

#include <entity/components/physics.hpp>
#include <entity/components/targeting/destination_targeting.hpp>
#include <entity/components/targeting/agent_targeting.hpp>
#include <entity/components/state_machine.hpp>
#include <entity/components/waiting.hpp>
#include <entity/macros.hpp>

namespace Entities
{

void go_to_next_destination(Entity* entity, Components::StateMachineComponent* machine)
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

void begin_wait(Entity* entity, Components::StateMachineComponent* machine)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, entity);
    waiting->tick = 0;
}

void waiting(Entity* entity, Components::StateMachineComponent* machine)
{
    auto waiting = GET_COMPONENT_INTERFACE(Waiting, entity);
    waiting->tick++;
    if (waiting->ready())
        machine->receive_event("done_waiting");
}

void in_transit(Entity* entity, Components::StateMachineComponent* machine)
{
    auto dest_target = GET_COMPONENT(DestinationTargeting, entity);

    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    // move towards target
    dest_target->move_on_surface();
    if (!physics->get_changed())
    {   // failed to move
        machine->receive_event("at_desination");
    }
    else
    {   // check at destination
        if (dest_target->check_at_destination())
        {
            if (dest_target->path_finished())
                machine->receive_event("at_desination");
            else
                go_to_next_destination(entity);
        }
        else
            dest_target->orient_to_target(physics->get_position());
    }
}

void chase_agent(Entity* entity, Components::StateMachineComponent* machine)
{   // TODO -- write a method that switches to STATE_WAITING under certain conditions
    // for slimes, this would be if we travelled too far from our original spawn location
    auto target = GET_COMPONENT(AgentTargeting, entity);

    // check target
    target->check_target_alive();
    if (target->target_type == NULL_ENTITY_TYPE)
    {
        machine->receive_event("agent_target_lost");
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

void wait_for_agent(Entity* entity, Components::StateMachineComponent* machine)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    Vec3 position = physics->get_position();

    auto target = GET_COMPONENT(AgentTargeting, entity);
    target->lock_target(position);

    if (target->target_type == ENTITY_AGENT)
        machine->receive_event("agent_targeted");
}

}   // Entities
