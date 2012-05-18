#include "./texture_loader.hpp"

#include <common/compat_gl.h>
#include <math.h>

#include <SDL/shader_loader.hpp>

// Note: Don't load surfaces via arguments, you must return SDL_Surface*

int init_image_loader()
{
    //printf("image loader init\n");
    IMG_Init(IMG_INIT_PNG); // IMG_INIT_JPG|IMG_INIT_PNG
    return 0;
}

SDL_Surface* _load_image(char *file)
{
    SDL_Surface *image;
    image=IMG_Load(file);
    if(!image)
    {
        printf("IMG_Load: %s \n", IMG_GetError());
        return 0;
    }
    if(image->format->BytesPerPixel != 4)
    {
        printf("IMG_Load: image is missing alpha channel \n");
        return 0;
    }

    return image;
}

SDL_Surface* create_surface_from_file(char* file)
{
    return _load_image(file);
}

struct Texture _load_image_create_texture(char *file)
{
    SDL_Surface *image = _load_image(file);

    Texture texture;

    if (image == NULL)
    {
        printf("ERROR: _load_image_create_texture, surface is NULL\n");
        return texture;
    }

    texture.w = image->w;
    texture.h = image->h;
    texture.tex = 0;
    int err = create_texture_from_surface(image, &texture.tex);
    if (err)
    {
        printf("_load_image_create_texture :: Texture could not be created from surface. Error code %d\n", err);
    }
    
    SDL_FreeSurface(image);
    
    return texture;
}

void _load_image_create_texture(char *file, struct Texture *tex)
{
    SDL_Surface *image = _load_image(file);

    if (image == NULL)
    {
        printf("ERROR: _load_image_create_texture, surface is NULL\n");
        return;
    }

    tex->w = image->w;
    tex->h = image->h;
    tex->tex = 0;
    int err = create_texture_from_surface(image, &tex->tex);
    if (err)
    {
        printf("_load_image_create_texture :: Texture could not be created from surface. Error code %d\n", err);
    }

    SDL_FreeSurface(image);
}

int create_texture_from_surface(SDL_Surface *surface, GLuint *tex)
{
    if(surface == NULL)
    {
        printf("Error: texture_loader.c create_texture_from_surface, surface is null!\n");
        return 1;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, tex );
    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, *tex );
    // Set the texture's stretching properties
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels ); //2nd parameter is level
    glDisable(GL_TEXTURE_2D);
    return 0;
}

int create_texture_from_file(char* filename, GLuint* tex)
{
    SDL_Surface *surface;
    surface=IMG_Load(filename);
    if (!surface)
    {
        *tex = 0;
        printf("Error loading texture %s, %s \n", filename, IMG_GetError());
        return 1;
    }
    if (surface->format->BytesPerPixel != 4)
    {
        *tex = 0;
        printf("IMG_Load: image is missing alpha channel \n");
        return 2;
    }
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, tex );
    glBindTexture( GL_TEXTURE_2D, *tex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    int texture_format;
    if (surface->format->Rmask == 0x000000ff)
    {
        texture_format = GL_RGBA;
    } else {
        texture_format = GL_BGRA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    return 0;
}

SDL_Surface* create_texture_and_surface_from_file(char* filename, GLuint* tex)
{
    SDL_Surface* surface;
    surface=IMG_Load(filename);
    if (surface==NULL)
    {
        printf("Error loading texture %s, %s \n", filename, IMG_GetError());
        return surface;
    }
    if (surface->format->BytesPerPixel != 4)
    {
        printf("IMG_Load: image is missing alpha channel \n");
        return 0;
    }
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, tex );
    glBindTexture( GL_TEXTURE_2D, *tex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    int texture_format;
    if (surface->format->Rmask == 0x000000ff)
    {
        texture_format = GL_RGBA;
    } else {
        texture_format = GL_BGRA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    return surface;
}

SDL_Surface* create_surface_from_nothing(int w, int h)
{
    // taken from http://sdl.beuc.net/sdl.wiki/SDL_CreateRGBSurface
    SDL_Surface* surface;

    static Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    //surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w,h, 32, rmask, gmask, bmask, amask);
    surface = SDL_CreateRGBSurface(SDL_SRCALPHA, w,h, 32, rmask, gmask, bmask, amask);
    //surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w,h, 32, rmask, gmask, bmask, amask);
    if(surface == NULL)
    {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
    }
    return surface;
}

void load_colored_texture(
    char* path, GLuint* texture,
    unsigned char br, unsigned char bg, unsigned char bb,   // base color
    unsigned char r, unsigned char g, unsigned char b       // replace with
)
{
    // get surface
    SDL_Surface* s = NULL;
    s = create_surface_from_file(path);
    if (s==NULL)
    {
        printf("ERROR: Failed to create surface for %s\n", path);
        return;
    }
    SDL_LockSurface(s);
    glEnable(GL_TEXTURE_2D);

    // alter surface, swapping base rgb with new rgb
    Uint32 pix;
    Uint8 sr,sg,sb,sa;
    for (int i=0; i<s->w*s->h; i++)
    {
        pix = ((Uint32*)s->pixels)[i];
        SDL_GetRGBA(pix, s->format, &sr, &sg, &sb, &sa);
        if (sr == br && sg == bg && sb == bb)
            ((Uint32*)s->pixels)[i] = SDL_MapRGBA(s->format, r,g,b,sa);
    }
    glDisable(GL_TEXTURE_2D);

    // create texture
    if (create_texture_from_surface(s, texture))
        printf("ERROR: failed to create texture from surface for %s\n", path);

    // cleanup
    SDL_UnlockSurface(s);
    SDL_FreeSurface(s);
}


void save_surface_to_png(SDL_Surface* surface, char* filename)
{
    size_t png_size;

    //void *tdefl_write_image_to_png_file_in_memory(
    //    const void *pImage, int w, int h, int num_chans, size_t *pLen_out);
    
    char* PNG_IMAGE;
    if( SDL_MUSTLOCK(surface) == 0 )
    {
        PNG_IMAGE = (char* ) tdefl_write_image_to_png_file_in_memory(
        (const char*) surface->pixels, surface->w, surface->h, 4, &png_size);
    }
    else
    {
        SDL_LockSurface(surface);
        PNG_IMAGE = (char* ) tdefl_write_image_to_png_file_in_memory(
        (const char*) surface->pixels, surface->w, surface->h, 4, &png_size);
        SDL_UnlockSurface(surface);
    }

    FILE* pFile = fopen(filename , "wb");
    fwrite (PNG_IMAGE, 1 , png_size, pFile);
    fclose (pFile);

    free(PNG_IMAGE);
} 
