#pragma once

#include <entity/components/physics.hpp>
#include <entity/components/targeting/destination_targeting.hpp>
#include <entity/components/targeting/agent_targeting.hpp>
#include <entity/components/state_machine.hpp>
#include <entity/components/waiting.hpp>

namespace Entities
{

static void go_to_next_destination(Entity* object)
{
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
    angles.x = vec3_to_theta(dest->target_direction);  // only rotate in x
    physics->set_angles(angles);
}

static void waiting_to_in_transit(Entity* object)
{
    go_to_next_destination(object);
    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_IN_TRANSIT;
}

static void waiting_to_chase_agent(Entity* object)
{   // assumes target already locked
    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_CHASE_AGENT;
}

static void in_transit_to_waiting(Entity* object)
{
    using Components::WaitingComponent;
    WaitingComponent* wait = (WaitingComponent*)object->get_component_interface(COMPONENT_INTERFACE_WAITING);
    wait->tick = 0;

    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_WAITING;
}

static void in_transit_to_chase_agent(Entity* object)
{   // assumes target already locked
    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_CHASE_AGENT;
}

static void chase_agent_to_waiting(Entity* object)
{
    using Components::WaitingComponent;
    WaitingComponent* wait = (WaitingComponent*)object->get_component_interface(COMPONENT_INTERFACE_WAITING);
    wait->tick = 0;

    using Components::StateMachineComponent;
    StateMachineComponent* state = (StateMachineComponent*)object->get_component_interface(COMPONENT_INTERFACE_STATE_MACHINE);
    state->state = STATE_WAITING;
}

static void chase_agent_to_in_transit(Entity* object)
{   // unused
    GS_ASSERT(false);
}

static void waiting(Entity* object)
{
    using Components::WaitingComponent;
    WaitingComponent* wait = (WaitingComponent*)object->get_component_interface(COMPONENT_INTERFACE_WAITING);
    wait->tick++;
    if (wait->ready())
        waiting_to_in_transit(object);
}

static void in_transit(Entity* object)
{
    using Components::DestinationTargetingComponent;
    DestinationTargetingComponent* dest_target = (DestinationTargetingComponent*)object->get_component(COMPONENT_DESTINATION_TARGETING);

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

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
    using Components::AgentTargetingComponent;
    AgentTargetingComponent* target = (AgentTargetingComponent*)object->get_component(COMPONENT_AGENT_TARGETING);

    // check target
    target->check_target_alive();
    if (target->target_type == ENTITY_NONE)
    {
        chase_agent_to_waiting(object);
        return;
    }

    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)
        object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
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
