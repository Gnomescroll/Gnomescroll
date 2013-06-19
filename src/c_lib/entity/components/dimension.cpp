#include "dimension.hpp"

#include <entity/components/physics.hpp>
#include <entity/object/object.hpp>

namespace Components
{

Vec3 DimensionComponent::get_camera_position()
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, this->object);
    if (physics == NULL) return NULL_POSITION;
    Vec3 position = physics->get_position();
    position.z += this->camera_height;
    return position;
}

} // Entities
