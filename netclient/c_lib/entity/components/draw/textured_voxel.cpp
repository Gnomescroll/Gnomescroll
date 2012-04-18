#include "billboard_sprite.hpp"

// TODO -- dont include this stuff on server at all

#if DC_CLIENT
#include <c_lib/draw/draw.hpp>
#endif

#include <c_lib/entity/components/physics.hpp>

namespace Components
{

void TexturedVoxelComponent::call()
{
    #if DC_CLIENT
    PhysicsComponent* state = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (state == NULL) return;
    drawTexturedMinivox(state->get_position(), this->forward, this->right, this->normal,
        this->sprite_index, this->tx, this->ty, this->pixel_margin);
    #endif
}


} // Components
