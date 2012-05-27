#include "texture.hpp"

#include <SDL/texture_loader.hpp>

namespace t_hud
{

SDL_Surface* ItemGridSlotSurface;
GLuint ItemGridSlotTexture;

GLuint NaniteTexture;
GLuint CraftingTexture;
GLuint StorageBlockTexture;

void init_nanite_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file((char*) "media/sprites/nanite_4.png", &NaniteTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    
    //SDL_Surface* s = create_surface_from_file((char*) "media/sprites/nanite_4.png");
    
    //if (s == NULL) printf("t_hud: init_nanite_texture spritesheet load error\n");
    //GS_ASSERT(s != NULL);
 
    //glEnable(GL_TEXTURE_2D);
    //glGenTextures( 1, &NaniteTexture );
    //glBindTexture( GL_TEXTURE_2D, NaniteTexture );

    ////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels );
    //glDisable(GL_TEXTURE_2D);

    //SDL_FreeSurface(s);
}

void init_crafting_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file((char*) "media/sprites/crafting_bench.png", &CraftingTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);

    //SDL_Surface* s;

    //s = create_surface_from_file((char*) "media/sprites/crafting_bench.png");
    
    //if (s == NULL) printf("t_hud: init_crafting_texture( spritesheet load error\n");
    //GS_ASSERT(s != NULL);
 
    //glEnable(GL_TEXTURE_2D);
    //glGenTextures( 1, &CraftingTexture );
    //glBindTexture( GL_TEXTURE_2D, CraftingTexture );

    ////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 
    //GLuint internalFormat = GL_RGBA;
    //GLuint format = GL_RGBA;

    //glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    //glDisable(GL_TEXTURE_2D);

    //SDL_FreeSurface(s);
}

void init_storage_block_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file((char*) "media/sprites/storage_block.png", &StorageBlockTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);

    //SDL_Surface* s;

    //s = create_surface_from_file((char*) "media/sprites/storage_block.png");
    
    //if (s == NULL) printf("t_hud: init_crafting_texture( spritesheet load error\n");
    //GS_ASSERT(s != NULL);
 
    //glEnable(GL_TEXTURE_2D);
    //glGenTextures( 1, &StorageBlockTexture );
    //glBindTexture( GL_TEXTURE_2D, StorageBlockTexture );

    ////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 
    //GLuint internalFormat = GL_RGBA;
    //GLuint format = GL_RGBA;

    //glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    //glDisable(GL_TEXTURE_2D);

    //SDL_FreeSurface(s);
}

void init_texture()
{
    init_nanite_texture();
    init_crafting_texture();
    init_storage_block_texture();
}

void teardown_texture()
{
}

}
