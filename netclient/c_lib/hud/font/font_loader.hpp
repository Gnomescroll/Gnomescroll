#pragma once


struct _font_meta* load_font(char* filename, int size);

/*
struct SDL_Surface* font_to_surface(class CFontSys* c)
{
    struct SDL_Surface* s = create_surface_from_nothing(c->font_texture_width, c->font_texture_height);

    if(bitmap == NULL)
    {
        printf("CFontSys::GenerateSDL_Surface: bull bitmap\n");
        return NULL;
    }
    SDL_LockSurface(s);
    for(int i=0; i< c->font_texture_width* c->font_texture_height; i++)
    {
        ((Uint32*)s->pixels)[i] = ((Uint32*) c->bitmap )[i];
    } 
    SDL_UnlockSurface(s);

    return s;
}


void font_to_file(class CFontSys* c, char* filename)
{
    struct SDL_Surface* s = GenerateSDL_Surface();
    
    if(s == NULL)
    {
        printf("CFontSys::save_font_to_file: null surface\n");
        return;
    }

    SDL_LockSurface(s);
    for(int i=0; i< c->font_texture_width* c->font_texture_height; i++)
    {
        ((Uint32*)s->pixels)[i] = ((Uint32*) c->bitmap )[i];
    } 
    SDL_UnlockSurface(s);
    IMG_SavePNG("test2.png", s, -1);

    IMG_SavePNG(filename, s, -1);

    SDL_FreeSurface(s);
}
*/