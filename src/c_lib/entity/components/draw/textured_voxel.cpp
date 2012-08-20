#include "billboard_sprite.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <camera/camera.hpp>
#include <common/draw/draw.hpp>
#include <entity/components/physics.hpp>

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

    this->sprite_width = pixel_w*(float)pix;

    int itx,ity;
    float ftx,fty;

    // integer index of cube in spritesheet
    itx = this->texture_index % cubes_per_surface_axis;
    ity = this->texture_index / cubes_per_surface_axis;

    // ...scaled to texture
    ftx = itx * scale;
    fty = ity * scale;

    int margin = cube_w - pix;
    GS_ASSERT(margin >= 0);

    int mx = randrange(0, margin);// - (margin+1);
    int my = randrange(0, margin);// - (margin+1);

    // ...scaled to texture
    float fx = ((float)mx) * pixel_w;
    float fy = ((float)my) * pixel_w;

    ftx += fx;
    fty += fy;

    this->tx = ftx;
    this->ty = fty;
}

inline void TexturedVoxelComponent::draw(Vec3 position)
{
    GS_ASSERT_LIMIT(false, 1);
    //position = quadrant_translate_position(current_camera_position, position);
    //Vec3 center = vec3_add(normal, vec3_add(right, vec3_add(forward, position)));
    //if (sphere_fulstrum_test(center.x, center.y, center.z, this->size) == false) return;

    //drawTexturedMinivox(position,
        //this->forward, this->right, this->normal,
        //this->tx, this->ty, this->sprite_width);
}

void TexturedVoxelComponent::call()
{
    PhysicsComponent* state = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (state == NULL) return;
    this->draw(state->get_position());
}

} // Components
