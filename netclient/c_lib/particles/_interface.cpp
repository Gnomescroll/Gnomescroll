#include "_interface.hpp"

//c#include <c_lib/particles/cspray.hpp>
#include <c_lib/particles/grenade.hpp>
#include <c_lib/particles/item_particle.hpp>

#include <c_lib/particles/net/StoC.hpp>

#if DC_CLIENT
#include <c_lib/particles/shrapnel.hpp>
#include <c_lib/particles/blood.hpp>
#include <c_lib/particles/minivox_colored.hpp>
#include <c_lib/particles/minivox_textured.hpp>
#include <c_lib/particles/billboard_text.hpp>
#include <c_lib/particles/billboard_text_hud.hpp>
#include <c_lib/particles/billboard_sprite.hpp>

#include <compat_gl.h>
#include <c_lib/SDL/texture_loader.hpp>
//#include <c_lib/camera/camera.hpp>

//#include <c_lib/SDL/draw_functions.hpp>
#endif 

#if DC_SERVER
#include <c_lib/particles/grenade_shrapnel.hpp>
#endif


namespace Particles
{

class Grenade_list* grenade_list = NULL;
class ItemParticle_list* item_particle_list = NULL;


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
    item_particle_list = new ItemParticle_list;

#if DC_CLIENT
    shrapnel_list = new Shrapnel_list;
    blood_list = new Blood_list;

    colored_minivox_list = new ColoredMinivox_list;
    textured_minivox_list = new TexturedMinivox_list;
    billboard_text_list = new BillboardText_list;
    billboard_text_hud_list = new BillboardTextHud_list;

    Particles::init_shrapnel();
#endif

#if DC_SERVER
    grenade_shrapnel_list = new Grenade_shrapnel_list;
#endif


}

void teardown_particles()
{
    delete grenade_list;
    delete item_particle_list;

#if DC_CLIENT
    delete shrapnel_list;
    delete blood_list;
    delete colored_minivox_list;
    delete textured_minivox_list;
    delete billboard_text_list;
    delete billboard_text_hud_list;

    Particles::teardown_shrapnel();
#endif

#if DC_SERVER
    delete grenade_shrapnel_list;
#endif
}


#if DC_CLIENT

GLuint particle_texture = 0;

void draw_init() 
{
    init_item_particle();
    create_texture_from_file((char*) "./media/texture/particles_01.png", &particle_texture);
}

void draw_teardown()
{
    teardown_item_particle();
}

void begin_particle_draw()
{
    assert(particle_texture != 0);
    GL_ASSERT(GL_TEXTURE_2D, true);

    glColor4ub(255,255,255,255);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
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

Shrapnel* create_shrapnel(float x, float y, float z, float vx, float vy, float vz)
{
    Shrapnel* s = shrapnel_list->create();
    if(s == NULL) return NULL;
    s->init(x,y,z, vx,vy,vz);
    return s;
}
#endif

#if DC_SERVER

class ItemParticle* create_item_particle(int item_type, 
    float x, float y, float z, 
    float vx, float vy, float vz)
{
    ItemParticle* ip = item_particle_list->create();
    if(ip == NULL)
    { 
        printf("Error: cannot create item particle \n");
        return NULL; 
    }
    ip->init(x,y,z,vx,vy,vz);

    class item_particle_create_StoC p;

    p.type = 0;
    p.id = ip->id;
    p.x = x;
    p.y = y;
    p.z = z;
    p.mx = vx;
    p.my = vy;
    p.mz = vz;

    p.broadcast();

    return ip;
}

#endif



}
