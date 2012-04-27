#include "texture.hpp"

namespace t_hud
{

SDL_Surface* ItemSheetSurface;
unsigned int ItemSheetTexture;

SDL_Surface* ItemGridSlotSurface;
unsigned int ItemGridSlotTexture;

void init_texture()
{

    SDL_Surface* s;

    GLuint internalFormat = GL_RGBA; //GL_RGBA;
    GLuint format = GL_RGBA;

    // Texture 1
    //s = create_surface_from_file((char*) "media/sprites/i00.png");    // this is 8x8
    s = create_surface_from_file((char*) "media/sprites/i01_256.png");      // this is 16x16

    if (s == NULL)
    {
        printf("t_hud: ItemSheetTexture spritesheet load error\n");
        assert(s != NULL);
        return;
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &ItemSheetTexture );
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture  );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);

    ItemSheetSurface = s;

    // Texture 2
    s = create_surface_from_file((char*) "media/sprites/inventory_slots.png");
    
    if (s == NULL)
    {
        printf("t_hud: ItemGridTileTexture spritesheet load error\n");
        assert(s != NULL);
        return;
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &ItemGridSlotTexture );
    glBindTexture( GL_TEXTURE_2D, ItemGridSlotTexture  );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);

    ItemGridSlotSurface = s;

}

void teardown_texture()
{


}

}
