#include "billboard_sprite.hpp"

#if DC_CLIENT

#include <c_lib/draw/draw.hpp>
#include <c_lib/entity/components/physics.hpp>

namespace Components
{

void TexturedVoxelComponent::set_texture()
{
    const int pix = this->pixel_width;  // NxN random texture sample within cube identified by texture_index
    const int cube_w = 32;
    const int cube_surface_w = 512;
    const int cubes_per_surface_axis = cube_surface_w / cube_w;
    const float scale = 2.0f/((float)cube_w);

    const float pixel_w = 1.0f/((float)cube_surface_w);

    this->pixel_margin = pixel_w*pix;

    int itx,ity;
    float ftx,fty;

    // integer index of cube in spritesheet
    itx = this->texture_index % cubes_per_surface_axis;
    ity = this->texture_index / cubes_per_surface_axis;

    // ...scaled to texture
    ftx = itx * scale;
    fty = ity * scale;

    int mx,my;
    float fx,fy;
    // random point within cube, with margins wide enough to fit pix
    mx = (rand() % (cube_w -(pix*2))) + pix;
    my = (rand() % (cube_w -(pix*2))) + pix;

    // ...scaled to texture
    fx = ((float)mx) * pixel_w;
    fy = ((float)my) * pixel_w;

    ftx += fx;
    fty += fy;

    this->tx = ftx;
    this->ty = fty;
}

void TexturedVoxelComponent::call()
{
    PhysicsComponent* state = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (state == NULL) return;
    drawTexturedMinivox(state->get_position(), this->forward, this->right, this->normal,
        this->texture_index, this->tx, this->ty, this->pixel_margin);
}

} // Components

#endif
