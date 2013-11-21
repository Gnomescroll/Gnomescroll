/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Particle
{

// state
void init_particles();
void teardown_particles();

extern class GrenadeList* grenade_list;

#if DC_CLIENT
extern class PlasmagenSpurList* plasmagen_spur_list;
extern class ShrapnelList* shrapnel_list;
extern class BloodList* blood_list;
extern class ColoredMinivoxList* colored_minivox_list;
extern class TexturedMinivoxList* textured_minivox_list;
extern class BillboardTextList* billboard_text_list;
extern class BillboardTextHudList* billboard_text_hud_list;
#endif

void destroy(int particle_id);

#if DC_CLIENT
class Shrapnel* create_shrapnel(struct Vec3 p, struct Vec3 v);
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

}   // Particle
