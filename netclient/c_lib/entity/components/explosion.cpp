#include "explosion.hpp"

#if DC_SERVER

#include <c_lib/state/server_state.hpp>
#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/agent_list.hpp>
#include <c_lib/entity/components/physics.hpp>
#include <c_lib/entity/components/owner.hpp>

namespace Components
{

void ExplosionComponent::explode()
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();

    using Components::OwnerComponent;
    OwnerComponent* owner = (OwnerComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    int owner_id = NO_AGENT;
    if (owner != NULL) owner_id = owner->get_owner();
    
    ServerState::damage_objects_within_sphere(
        position.x, position.y, position.z, this->radius, this->damage, owner_id,
        this->object->type, this->object->id, this->suicidal
    );
}

// returns true if within proximity explosion range
bool ExplosionComponent::proximity_check()
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();
    
    Agent_state* agent = nearest_agent_in_range(position, this->proximity_radius);
    if (agent != NULL)
    {
        using Components::HealthComponent;
        HealthComponent* health = (HealthComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_HEALTH);
        if (health != NULL) health->die();
        return true;
    }
    return false;
}


} // Components

#endif
