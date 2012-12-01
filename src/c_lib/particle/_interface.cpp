#include "_interface.hpp"

#include <particle/grenade.hpp>
#include <particle/plasmagen_spur.hpp>

#if DC_CLIENT
# include <particle/shrapnel.hpp>
# include <particle/blood.hpp>
# include <particle/minivox_colored.hpp>
# include <particle/minivox_textured.hpp>
# include <particle/text/billboard_text.hpp>
# include <particle/text/billboard_text_hud.hpp>
# include <particle/billboard_sprite.hpp>

# include <common/compat_gl.h>
# include <SDL/texture_loader.hpp>
#endif 

namespace Particle
{

class GrenadeList* grenade_list = NULL;

#if DC_CLIENT
class PlasmagenSpurList* plasmagen_spur_list = NULL;
class Shrapnel_list* shrapnel_list = NULL;
class Blood_list* blood_list = NULL;
class ColoredMinivox_list* colored_minivox_list = NULL;
class TexturedMinivox_list* textured_minivox_list = NULL;
class BillboardText_list* billboard_text_list = NULL;
class BillboardTextHud_list* billboard_text_hud_list = NULL;
#endif

void init_particles()
{
    grenade_list = new GrenadeList(MAX_GRENADES);

    #if DC_CLIENT
    plasmagen_spur_list = new PlasmagenSpurList(PLASMAGEN_SPUR_MAX);
    shrapnel_list = new Shrapnel_list;
    blood_list = new Blood_list;

    colored_minivox_list = new ColoredMinivox_list;
    textured_minivox_list = new TexturedMinivox_list;
    billboard_text_list = new BillboardText_list;
    billboard_text_hud_list = new BillboardTextHud_list;

    Particle::init_shrapnel();
    #endif
}

void teardown_particles()
{
    if (grenade_list != NULL) delete grenade_list;

    #if DC_CLIENT
    if (plasmagen_spur_list != NULL) delete plasmagen_spur_list;
    if (shrapnel_list != NULL) delete shrapnel_list;
    if (blood_list != NULL) delete blood_list;
    if (colored_minivox_list != NULL) delete colored_minivox_list;
    if (textured_minivox_list != NULL) delete textured_minivox_list;
    if (billboard_text_list != NULL) delete billboard_text_list;
    if (billboard_text_hud_list != NULL) delete billboard_text_hud_list;

    Particle::teardown_shrapnel();
    #endif
}

#if DC_CLIENT
GLuint particle_texture = 0;

void draw_init() 
{
    create_texture_from_file("./media/sprites/animation/particles_01.png", &particle_texture);
    GS_ASSERT(particle_texture != 0);
}

void draw_teardown()
{
}

void begin_particle_draw()
{
    GS_ASSERT(particle_texture != 0);
    if (particle_texture == 0) return;
    
    glColor4ub(255,255,255,255);
    
    GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    GL_ASSERT(GL_BLEND, true);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, particle_texture);
    

    glBegin(GL_QUADS);
}

void end_particle_draw()
{
    glEnd();
    //glDisable(GL_BLEND);
}


void prep_shrapnel()
{
    shrapnel_list->prep();
}

void draw_shrapnel()
{
    shrapnel_list->draw();
}

class Shrapnel* create_shrapnel(float x, float y, float z, float vx, float vy, float vz)
{
    class Shrapnel* s = shrapnel_list->create();
    if(s == NULL) return NULL;
    s->set_state(x,y,z, vx,vy,vz);
    return s;
}
#endif

}   // Particle
