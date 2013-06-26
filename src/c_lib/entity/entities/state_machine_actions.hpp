#pragma once

namespace Components
{
class StateMachineComponent;
}

namespace Entities
{

class Entity;

void check_agent_aggro(Entity* entity, class Components::StateMachineComponent* machine);
void go_to_next_destination(Entity* entity, class Components::StateMachineComponent* machine);
void begin_wait(Entity* entity, class Components::StateMachineComponent* machine);
void do_wait(Entity* entity, class Components::StateMachineComponent* machine);
void in_transit(Entity* entity, class Components::StateMachineComponent* machine);
void chase_agent(Entity* entity, class Components::StateMachineComponent* machine);
void stick_to_surface(Entity* entity, class Components::StateMachineComponent* machine);

}   // Entities
