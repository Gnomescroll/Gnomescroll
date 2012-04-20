#include "explosion.hpp"

#if DC_SERVER

#include <c_lib/state/server_state.hpp>
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
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    int owner_id = NO_AGENT;
    if (owner != NULL) owner_id = owner->get_owner();
    
    ServerState::damage_objects_within_sphere(
        position.x, position.y, position.z, this->radius, this->damage, owner_id,
        this->object->type, this->object->id, this->suicidal
    );
}

} // Components

#endif
