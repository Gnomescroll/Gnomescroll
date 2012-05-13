#include "_interface.hpp"

#include <particle/grenade.hpp>

#if DC_CLIENT
#include <particle/shrapnel.hpp>
#include <particle/blood.hpp>
#include <particle/minivox_colored.hpp>
#include <particle/minivox_textured.hpp>
#include <particle/billboard_text.hpp>
#include <particle/billboard_text_hud.hpp>
#include <particle/billboard_sprite.hpp>

#include <common/compat_gl.h>
#include <SDL/texture_loader.hpp>
//#include <camera/camera.hpp>

//#include <SDL/draw_functions.hpp>
#endif 

#if DC_SERVER
#include <particle/grenade_shrapnel.hpp>
#endif


namespace Particle
{

class Grenade_list* grenade_list = NULL;

#if DC_CLIENT
class Shrapnel_list* shrapnel_list = NULL;
class Blood_list* blood_list = NULL;
class ColoredMinivox_list* colored_minivox_list = NULL;
class TexturedMinivox_list* textured_minivox_list = NULL;
class BillboardText_list* billboard_text_list = NULL;
class BillboardTextHud_list* billboard_text_hud_list = NULL;
#endif

#if DC_SERVER
class Grenade_shrapnel_list* grenade_shrapnel_list = NULL;
#endif

void init_particles()
{
    grenade_list = new Grenade_list;

    #if DC_CLIENT
    shrapnel_list = new Shrapnel_list;
    blood_list = new Blood_list;

    colored_minivox_list = new ColoredMinivox_list;
    textured_minivox_list = new TexturedMinivox_list;
    billboard_text_list = new BillboardText_list;
    billboard_text_hud_list = new BillboardTextHud_list;

    Particle::init_shrapnel();
    #endif

    #if DC_SERVER
    grenade_shrapnel_list = new Grenade_shrapnel_list;
    #endif


}

void teardown_particles()
{
    delete grenade_list;

    #if DC_CLIENT
    delete shrapnel_list;
    delete blood_list;
    delete colored_minivox_list;
    delete textured_minivox_list;
    delete billboard_text_list;
    delete billboard_text_hud_list;

    Particle::teardown_shrapnel();
    #endif

    #if DC_SERVER
    delete grenade_shrapnel_list;
    #endif
}


#if DC_CLIENT

GLuint particle_texture = 0;

void draw_init() 
{
    create_texture_from_file((char*) "./media/texture/particles_01.png", &particle_texture);
}

void draw_teardown()
{
}

void begin_particle_draw()
{
    assert(particle_texture != 0);

    glColor4ub(255,255,255,255);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, particle_texture);
    glBegin(GL_QUADS);
}
void end_particle_draw()
{
    glEnd();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}


void prep_shrapnel()
{
    shrapnel_list->prep();
}

void draw_shrapnel()
{
    shrapnel_list->draw();
}

// TODO -- RESTORE SHRAPNEL ONCE RENDERING IS FIXED
Shrapnel* create_shrapnel(float x, float y, float z, float vx, float vy, float vz)
{
    return NULL;
    
    //Shrapnel* s = shrapnel_list->create();
    //if(s == NULL) return NULL;
    //s->init(x,y,z, vx,vy,vz);
    //return s;
}
#endif

}
