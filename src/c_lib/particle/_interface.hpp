#pragma once

namespace Particle
{

// state
void init_particles();
void teardown_particles();

extern class Grenade_list* grenade_list;

#if DC_CLIENT
extern class  Shrapnel_list* shrapnel_list;
extern class  Blood_list* blood_list;
extern class  ColoredMinivox_list* colored_minivox_list;
extern class  TexturedMinivox_list* textured_minivox_list;
extern class  BillboardText_list* billboard_text_list;
extern class  BillboardTextHud_list* billboard_text_hud_list;
#endif

void destroy(int particle_id);

#if DC_CLIENT
class Shrapnel* create_shrapnel(float x, float y, float z, float vx, float vy, float vz);
#endif

#if DC_CLIENT
// rendering
extern GLuint particle_texture;
void draw_init();
void draw_teardown();

void begin_particle_draw();
void end_particle_draw();

void prep_shrapnel();
void draw_shrapnel();
#endif
}
