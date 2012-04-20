#include "explosion.hpp"

#if DC_SERVER

#include <c_lib/state/server_state.hpp>

namespace Components
{

void ExplosionComponent::explode(Vec3 position, int owner)
{
    ServerState::damage_objects_within_sphere(
        position.x, position.y, position.z, this->radius, this->damage, owner,
        this->object->type, this->object->id, this->suicidal
    );
}

} // Components

#endif
