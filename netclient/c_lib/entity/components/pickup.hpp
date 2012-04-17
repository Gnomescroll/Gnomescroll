#pragma once

#include <c_lib/agent/constants.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class PickupComponent: public Component
{
    public:
        int pickup_delay;
        float pickup_radius;
        int picked_up_by;
        bool picked_up;

        void was_picked_up(const int agent_id)
        {
            //this->broadcast_death = true;
            this->picked_up = true;
            this->picked_up_by = agent_id;
        }

        void was_dropped()
        {
            this->pickup_delay = 30 * 5;
        }

        void tick(Object* object)
        {
            #if DC_SERVER
            if (this->pickup_delay > 0)
            {
                this->pickup_delay--;
                return;
            }
            PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            Vec3 position = physics->get_position();
            Agent_state* agent = nearest_agent_in_range(position, this->pickup_radius);
            if (agent != NULL && agent->status.gain_item(object->id, (Object_types)object->type, 0))  // todo remove subtype
            {   // was picked up, die
                this->was_picked_up(agent->id);
                //HealthComponent* health = (HealthComponent*)object->get_component_interface(COMPONENT_INTERFACE_HEALTH);
                //health->set_dead(true);
            }
            #endif
        }

    PickupComponent()
    : Component(COMPONENT_PICKUP, COMPONENT_INTERFACE_PICKUP),
        pickup_delay(0), pickup_radius(1.0f),
        picked_up_by(NO_AGENT), picked_up(false)
    {}
};

} // Components
