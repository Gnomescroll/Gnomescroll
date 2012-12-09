#include "./texture_loader.hpp"

#include <common/compat_gl.h>
#include <math.h>

#include <SDL/shader_loader.hpp>

// Note: Don't load surfaces via arguments, you must return SDL_Surface*

int init_image_loader()
{
    IMG_Init(IMG_INIT_PNG); // IMG_INIT_JPG|IMG_INIT_PNG
    return 0;
}

SDL_Surface* _load_image(const char* file)
{
    SDL_Surface* image = IMG_Load(file);
    if (!image)
    {
        printf("Failed to load %s. IMG_Load error: %s\n", file, IMG_GetError());
        return NULL;
    }
    GS_ASSERT(image->format->BytesPerPixel == 4);
    if (image->format->BytesPerPixel != 4)
    {
        printf("IMG_Load: image %s is missing alpha channel. has %d bytes per pixel\n", file, image->format->BytesPerPixel);
        return NULL;
    }

    return image;
}

SDL_Surface* create_surface_from_file(const char* file)
{
    return _load_image(file);
}

void _load_image_create_texture(const char* file, struct Texture *tex)
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
    return create_texture_from_surface(surface, tex, GL_LINEAR, GL_LINEAR);
}

// TODO -- deprecate
int create_texture_from_surface(SDL_Surface *surface, GLuint *tex, GLuint mag_filter)
{
    return create_texture_from_surface(surface, tex, GL_LINEAR, mag_filter);
}

int create_texture_from_surface(SDL_Surface *surface, GLuint *tex, GLuint min_filter, GLuint mag_filter)
{
    GS_ASSERT(surface->format->BytesPerPixel == 4);
    if (surface->format->BytesPerPixel != 4)
    {
        printf("Surface is missing alpha channel\n");
        return 1;
    }
    
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum texture_format;
    if (surface->format->Rmask == 0x000000ff)
        texture_format = GL_RGBA;
    else
        texture_format = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);

    glDisable(GL_TEXTURE_2D);

    return 0;
}

int create_texture_from_file(const char* filename, GLuint* tex)
{
    return create_texture_from_file(filename, tex, GL_LINEAR, GL_LINEAR);
}

int create_texture_from_file(const char* filename, GLuint* tex, GLuint min_filter, GLuint mag_filter)
{
    SDL_Surface* s = create_texture_and_surface_from_file(filename, tex, min_filter, mag_filter);
    if (s == NULL) return 1;
    SDL_FreeSurface(s);
    return 0;
}

SDL_Surface* create_texture_and_surface_from_file(const char* filename, GLuint* tex)
{
    return create_texture_and_surface_from_file(filename, tex, GL_LINEAR, GL_LINEAR);
}

SDL_Surface* create_texture_and_surface_from_file(const char* filename, GLuint* tex, GLuint min_filter, GLuint mag_filter)
{
    GS_ASSERT(tex != NULL);
    if (tex == NULL) return NULL;
    *tex = 0;
    
    SDL_Surface* surface = IMG_Load(filename);
    GS_ASSERT(surface != NULL);
    if (surface == NULL)
    {
        printf("Error loading texture %s, %s\n", filename, IMG_GetError());
        return NULL;
    }
    
    int ret = create_texture_from_surface(surface, tex, min_filter, mag_filter);
    if (ret != 0)
    {
        SDL_FreeSurface(surface);
        return NULL;
    }
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

    surface = SDL_CreateRGBSurface(SDL_SRCALPHA, w,h, 32, rmask, gmask, bmask, amask);
    if (surface == NULL)
    {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
    }
    return surface;
}

void load_colored_texture(
    const char* path, GLuint* texture,
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


void save_surface_to_png(SDL_Surface* surface, const char* filename)
{
    size_t png_size;

    //void *tdefl_write_image_to_png_file_in_memory(
    //    const void *pImage, int w, int h, int num_chans, size_t *pLen_out);
    
    char* png_data = NULL;
    if (SDL_MUSTLOCK(surface) == 0)
    {
        png_data = (char*)tdefl_write_image_to_png_file_in_memory(
        (const char*)surface->pixels, surface->w, surface->h, 4, &png_size);
    }
    else
    {
        SDL_LockSurface(surface);
        png_data = (char*)tdefl_write_image_to_png_file_in_memory(
        (const char*)surface->pixels, surface->w, surface->h, 4, &png_size);
        SDL_UnlockSurface(surface);
    }
    
    GS_ASSERT(png_data != NULL);
    if (png_data == NULL) return;

    FILE* pFile = fopen(filename, "wb");
    fwrite(png_data, 1, png_size, pFile);
    fclose(pFile);

    free(png_data);
} 
