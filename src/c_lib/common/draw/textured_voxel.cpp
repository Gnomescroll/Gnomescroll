#include "textured_voxel.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <camera/camera.hpp>
#include <common/draw/draw.hpp>

namespace Draw
{

void TexturedVoxel::set_texture()
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

} // Draw
