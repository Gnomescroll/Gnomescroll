#include "texture.hpp"

namespace t_hud
{

//SDL_Surface* ItemSheetSurface;
unsigned int ItemSheetTexture;

SDL_Surface* ItemGridSlotSurface;
unsigned int ItemGridSlotTexture;

unsigned int NaniteTexture;

unsigned int CraftingTexture;

void init_item_sheet();
void init_nanite_texture();
void init_crafting_texture();

void init_texture()
{
    init_item_sheet();
    init_nanite_texture();
    init_crafting_texture();
}

void init_item_sheet()
{
    SDL_Surface* s = TextureSheetLoader::ItemSurface;

    if (s == NULL) printf("ItemContainer::init_item_sheet, error \n");
    assert(s != NULL);

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &ItemSheetTexture );

    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels ); //2nd parameter is level
    
    glDisable(GL_TEXTURE_2D);
}

void init_nanite_texture()
{
    SDL_Surface* s = create_surface_from_file((char*) "media/sprites/nanite_4.png");
    
    if (s == NULL) printf("t_hud: init_nanite_texture spritesheet load error\n");
    assert(s != NULL);
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &NaniteTexture );
    glBindTexture( GL_TEXTURE_2D, NaniteTexture );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);

    //NaniteSurface = s;
}

void init_crafting_texture()
{
    SDL_Surface* s;

    s = create_surface_from_file((char*) "media/sprites/crafting_bench.png");
    
    if (s == NULL) printf("t_hud: init_crafting_texture( spritesheet load error\n");
    assert(s != NULL);
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &CraftingTexture );
    glBindTexture( GL_TEXTURE_2D, CraftingTexture );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 
    GLuint internalFormat = GL_RGBA;
    GLuint format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);
}

void teardown_texture()
{


}

}
