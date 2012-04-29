#include "item_particle.hpp"

#include <particle/net/StoC.hpp>
#include <item/_interface.hpp>

#include <item/common/constant.hpp>

namespace Particle
{

#ifdef DC_CLIENT

SDL_Surface* ItemSheetSurface = NULL;
unsigned int ItemSheetTexture = 0;

void init_item_particle()
{

    SDL_Surface* s = create_surface_from_file((char*) "media/sprites/i01_256.png");
    if(s == NULL)
    {
        printf("t_item: spritesheet load error\n");
        abort();
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &ItemSheetTexture );
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture  );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    GLuint internalFormat = GL_RGBA; //GL_RGBA;
    GLuint format = GL_RGBA;
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);

    ItemSheetSurface = s;
}

void teardown_item_particle()
{
    glDeleteTextures(1, &ItemSheetTexture );
    SDL_FreeSurface(ItemSheetSurface);
}

void ItemParticle::draw()
{
    const float scale = 0.25;
    const float h = 0.35;

    Vec3 up = vec3_init(
        model_view_matrix[0]*scale,
        model_view_matrix[4]*scale,
        model_view_matrix[8]*scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*scale,
        model_view_matrix[5]*scale,
        model_view_matrix[9]*scale
    );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->sprite_index%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(this->sprite_index/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    Vec3 position = verlet.position;
    Vec3 p = vec3_sub(position, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z+h);
}
#endif

void ItemParticle::die()
{
    #if DC_SERVER
    if (this->was_picked_up) return; // already send a packet
    class item_particle_destroy_StoC msg;
    msg.id = this->id;
    msg.broadcast();
    #endif
}

#if DC_CLIENT
void ItemParticle::init(int item_type, float x, float y, float z, float mx, float my, float mz)
#endif
#if DC_SERVER
void ItemParticle::init(ItemID item_id, int item_type, float x, float y, float z, float mx, float my, float mz)
#endif
{
    this->item_type = item_type;
    #if DC_CLIENT
    this->sprite_index = Item::get_sprite_index_for_type(item_type);
    #endif
    #if DC_SERVER
    this->item_id = item_id;
    #endif
    verlet.position = vec3_init(x,y,z);
    verlet.velocity = vec3_init(mx,my,mz);
}

#if DC_SERVER
void ItemParticle::picked_up(int agent_id)
{
    this->ttl = 0;
    this->was_picked_up = true;
    item_particle_picked_up_StoC msg;
    msg.id = this->id;
    msg.agent_id = agent_id;
    msg.broadcast();
}
#endif

ItemParticle::ItemParticle(int id)
:   id(id),
    item_type(NULL_ITEM_TYPE),
    #if DC_SERVER
    item_id(NULL_ITEM),
    pickup_prevention(0),
    was_picked_up(false),
    #endif
    #if DC_CLIENT
    sprite_index(ERROR_SPRITE),
    #endif
    ttl(ITEM_PARTICLE_TTL)
{
    verlet.dampening = ITEM_PARTICLE_DAMPENING;
}


}
