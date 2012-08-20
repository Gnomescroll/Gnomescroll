#include "billboard_sprite.hpp"

#if DC_CLIENT

#include <common/draw/draw.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

void ColoredVoxelComponent::call()
{
    GS_ASSERT_LIMIT(false, 1);
    //PhysicsComponent* state = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    //if (state == NULL) return;
    //drawColoredMinivox(state->get_position(), this->forward, this->right, this->normal, this->color);
}

} // Components

#endif
