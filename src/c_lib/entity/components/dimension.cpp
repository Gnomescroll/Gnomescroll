#include "dimension.hpp"

#include <entity/components/physics.hpp>
#include <entity/object/object.hpp>

namespace Components
{

Vec3 DimensionComponent::get_camera_position()
{
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return NULL_POSITION;
    Vec3 position = physics->get_position();
    position.z += this->camera_height;
    return position;
}

} // Entities
