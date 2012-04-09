#pragma once

namespace Particles
{

extern GLuint particle_texture;

void init_for_draw();
void draw_teardown();

void begin_particle_draw();
void end_particle_draw();

}