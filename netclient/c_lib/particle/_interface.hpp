#pragma once

namespace Particle
{

#ifdef DC_CLIENT

extern GLuint particle_texture;

void draw_init();
void draw_teardown();

void begin_particle_draw();
void end_particle_draw();

#endif

//extern class Cspray_list* cspray_list;
extern class Grenade_list* grenade_list;
extern class ItemParticle_list* item_particle_list;

#if DC_CLIENT

extern class  Shrapnel_list* shrapnel_list;
extern class  Blood_list* blood_list;
extern class  ColoredMinivox_list* colored_minivox_list;
extern class  TexturedMinivox_list* textured_minivox_list;
extern class  BillboardText_list* billboard_text_list;
extern class  BillboardTextHud_list* billboard_text_hud_list;
#endif

#if DC_SERVER
extern class  Grenade_shrapnel_list* grenade_shrapnel_list;
#endif

void init_particles();
void teardown_particles();



#if DC_CLIENT

void prep_shrapnel();
void draw_shrapnel();

class Shrapnel* create_shrapnel(float x, float y, float z, float vx, float vy, float vz);
#endif

#if DC_SERVER

class ItemParticle* create_item_particle(int item_type, 
    float x, float y, float z, 
    float vx, float vy, float vz);

#endif

}