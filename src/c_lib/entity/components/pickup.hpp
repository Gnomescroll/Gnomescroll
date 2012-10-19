#pragma once

#include <agent/constants.hpp>
#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <entity/components/health.hpp>

// TODO == move net packet
#include <entity/network/packets.hpp>

namespace Components
{

class PickupComponent: public Component
{
    public:
        int pickup_delay;
        float pickup_radius;
        int picked_up_by;
        bool picked_up;

        void was_picked_up(const AgentID agent_id)
        {
            //this->broadcast_death = true;
            this->picked_up = true;
            this->picked_up_by = agent_id;
        }

        void was_dropped()
        {
            this->pickup_delay = 30 * 5;
        }

        void tick()
        {
            //#if DC_SERVER
            //if (this->pickup_delay > 0)
            //{
                //this->pickup_delay--;
                //return;
            //}
            //PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            //Vec3 position = physics->get_position();

            //int stack_size = 1;
            //StackableComponent* stack = (StackableComponent*)object->get_component_interface(COMPONENT_INTERFACE_STACKABLE);
            //if (stack != NULL) stack_size = stack->count;
            
            //Agent* agent = nearest_agent_in_range(position, this->pickup_radius);
            //if (agent != NULL && agent->status.gain_stack_item(this->object->type, this->object->id, stack_size))
            //{   // was picked up, die
                //this->was_picked_up(agent->id);
                //HealthComponent* health = (HealthComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_HEALTH);
                //health->die();
            //}
            //#endif
        }

        void broadcast()
        {
            if (!this->picked_up || this->picked_up_by == NULL_AGENT) return;
            object_picked_up_StoC msg;
            msg.id = this->object->id;
            msg.type = this->object->type;
            msg.agent_id = this->picked_up_by;
            msg.broadcast();
        }

    PickupComponent()
    : Component(COMPONENT_PICKUP, COMPONENT_INTERFACE_PICKUP),
        pickup_delay(30), pickup_radius(1.0f),
        picked_up_by(NULL_AGENT), picked_up(false)
    {}
};

} // Components
