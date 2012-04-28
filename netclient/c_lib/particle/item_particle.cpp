#include "item_particle.hpp"

#include <particle/net/StoC.hpp>
#include <item/_interface.hpp>

namespace Particle
{

#ifdef DC_CLIENT

SDL_Surface* ItemSheetSurface = NULL;
unsigned int ItemSheetTexture = 0;

void init_item_particle()
{

    SDL_Surface* s = create_surface_from_file((char*) "media/sprites/i00.png");
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

#endif

void ItemParticle::die()
{
    #if DC_SERVER
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


}
