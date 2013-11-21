/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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
class ShrapnelList* shrapnel_list = NULL;
class BloodList* blood_list = NULL;
class ColoredMinivoxList* colored_minivox_list = NULL;
class TexturedMinivoxList* textured_minivox_list = NULL;
class BillboardTextList* billboard_text_list = NULL;
class BillboardTextHudList* billboard_text_hud_list = NULL;
#endif

void init_particles()
{
    grenade_list = new GrenadeList(MAX_GRENADES);
    #if DC_CLIENT
    plasmagen_spur_list = new PlasmagenSpurList(PLASMAGEN_SPUR_MAX);
    shrapnel_list = new ShrapnelList;
    blood_list = new BloodList;
    colored_minivox_list = new ColoredMinivoxList;
    textured_minivox_list = new TexturedMinivoxList;
    billboard_text_list = new BillboardTextList;
    billboard_text_hud_list = new BillboardTextHudList;
    Particle::init_shrapnel();
    #endif
}

void teardown_particles()
{
    delete grenade_list;
    #if DC_CLIENT
    delete plasmagen_spur_list;
    delete shrapnel_list;
    delete blood_list;
    delete colored_minivox_list;
    delete textured_minivox_list;
    delete billboard_text_list;
    delete billboard_text_hud_list;
    Particle::teardown_shrapnel();
    #endif
}

#if DC_CLIENT
GLuint particle_texture = 0;

void draw_init()
{
    create_texture_from_file(MEDIA_PATH "sprites/animation/particles_01.png", &particle_texture);
    GS_ASSERT(particle_texture != 0);
}

void draw_teardown()
{
}

void begin_particle_draw()
{
    IF_ASSERT(particle_texture == 0) return;

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
}

void prep_shrapnel()
{
    shrapnel_list->prep();
}

void draw_shrapnel()
{
    shrapnel_list->draw();
}

class Shrapnel* create_shrapnel(struct Vec3 p, struct Vec3 v)
{
    class Shrapnel* s = shrapnel_list->create();
    if (s == NULL) return NULL;
    s->set_state(p, v);
    return s;
}
#endif

}   // Particle
