#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

namespace Animations
{

void init_voxel_particle();
void teardown_voxel_particle();
void prep_voxel_particles();
void draw_textured_voxel_particles();
void draw_colored_voxel_particles();

}   // Animations
