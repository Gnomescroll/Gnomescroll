#pragma once

namespace Components
{
class StateMachineComponent;
}

namespace Entities
{

class Entity;

void check_agent_aggro(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);
void go_to_next_destination(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);
void begin_wait(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);
void do_wait(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);
void in_transit(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);
void chase_agent(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);
void stick_to_surface(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);
void target_attacker(Entity* entity, class Components::StateMachineComponent* machine, void* event_data);

}   // Entities
