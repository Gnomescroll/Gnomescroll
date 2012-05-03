#include "billboard_sprite.hpp"

#if DC_CLIENT

#include <common/draw/draw.hpp>
#include <entity/components/physics.hpp>

namespace Components
{

void BillboardSpriteComponent::call()
{
    #if DC_CLIENT
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    drawBillboardSprite(physics->get_position(), this->sprite_index, this->scale);
    #endif
}

} // Components

#endif
