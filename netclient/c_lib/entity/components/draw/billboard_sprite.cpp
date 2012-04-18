#include "billboard_sprite.hpp"

// TODO -- dont include this stuff on server at all

#if DC_CLIENT
#include <c_lib/draw/draw.hpp>
#endif

#include <c_lib/entity/components/physics.hpp>

namespace Components
{

void BillboardSpriteComponent::call()
{
    #if DC_CLIENT
    PhysicsComponent* state = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (state == NULL) return;
    drawBillboardSprite(state->get_position(), this->sprite_index, this->scale);
    #endif
}


} // Components
