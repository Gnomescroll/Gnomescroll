#include "_interface.hpp"

#include <particle/grenade.hpp>
#include <particle/item_particle.hpp>

#include <particle/net/StoC.hpp>

#if DC_CLIENT
#include <particle/shrapnel.hpp>
#include <particle/blood.hpp>
#include <particle/minivox_colored.hpp>
#include <particle/minivox_textured.hpp>
#include <particle/billboard_text.hpp>
#include <particle/billboard_text_hud.hpp>
#include <particle/billboard_sprite.hpp>

#include <compat_gl.h>
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

    Particle::init_shrapnel();
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

Shrapnel* create_shrapnel(float x, float y, float z, float vx, float vy, float vz)
{
    Shrapnel* s = shrapnel_list->create();
    if(s == NULL) return NULL;
    s->init(x,y,z, vx,vy,vz);
    return s;
}
#endif


#if DC_CLIENT
ItemParticle* create_item_particle(
    int particle_id, int item_type,
    float x, float y, float z, 
    float vx, float vy, float vz
) {
    ItemParticle* ip = item_particle_list->create(particle_id);
    if (ip == NULL) return NULL; 
    ip->init(item_type, x,y,z,vx,vy,vz);
    return ip;
}
#endif

#if DC_SERVER
ItemParticle* create_item_particle(
    ItemID item_id, int item_type,
    float x, float y, float z, 
    float vx, float vy, float vz
) {
    ItemParticle* ip = item_particle_list->create();
    if (ip == NULL) return NULL; 
    ip->init(item_id, item_type, x,y,z,vx,vy,vz);
    return ip;
}
#endif

#if DC_SERVER
void broadcast_particle_item_create(int particle_id)
{
    ItemParticle* particle = item_particle_list->get(particle_id);
    assert(particle != NULL);

    item_particle_create_StoC msg;
    msg.id = particle->id;
    msg.item_type = particle->item_type;
    msg.x = particle->verlet.position.x;
    msg.y = particle->verlet.position.y;
    msg.z = particle->verlet.position.z;
    msg.mx = particle->verlet.velocity.x;
    msg.my = particle->verlet.velocity.y;
    msg.mz = particle->verlet.velocity.z;
    msg.broadcast();
}
#endif

void destroy(int particle_id)
{
    item_particle_list->destroy(particle_id);
}


}
