#include "texture_loader.hpp"

#include <math.h>
#include <common/compat_gl.h>
#include <SDL/shader_loader.hpp>

// Note: Don't load surfaces via arguments, you must return SDL_Surface*

GLenum get_texture_format(SDL_Surface* surface)
{
    if (surface->format->Rmask == 0x000000FF)
        return  GL_RGBA;
    return GL_BGRA;
}

int init_image_loader()
{
    IMG_Init(IMG_INIT_PNG); // IMG_INIT_JPG|IMG_INIT_PNG
    return 0;
}

SDL_Surface* _load_image(const char* file)
{
    SDL_Surface* image = IMG_Load(file);
    IF_ASSERT(!image)
    {
        printf("Failed to load %s. IMG_Load error: %s\n", file, IMG_GetError());
        return NULL;
    }
    IF_ASSERT(image->format->BytesPerPixel != 4)
    {
        printf("ERROR: image %s is missing alpha channel\n", file);
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
    IF_ASSERT(image == NULL) return;

    tex->w = image->w;
    tex->h = image->h;
    tex->tex = 0;
    int err = create_texture_from_surface(image, &tex->tex);
    IF_ASSERT(err)
        printf("Could not load texture from file %s\n", file);

    SDL_FreeSurface(image);
}

int create_texture_from_surface(SDL_Surface *surface, GLuint *tex)
{
    return create_texture_from_surface(surface, tex, GL_LINEAR, GL_LINEAR);
}

int create_texture_from_surface(SDL_Surface *surface, GLuint *tex, GLuint mag_filter)
{
    return create_texture_from_surface(surface, tex, GL_LINEAR, mag_filter);
}

int create_texture_from_surface(SDL_Surface *surface, GLuint *tex, GLuint min_filter, GLuint mag_filter)
{
    IF_ASSERT(surface->format->BytesPerPixel != 4)
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

    GLenum texture_format = get_texture_format(surface);
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
    if (s == NULL)  return 1;
    SDL_FreeSurface(s);
    return 0;
}

SDL_Surface* create_texture_and_surface_from_file(const char* filename, GLuint* tex)
{
    return create_texture_and_surface_from_file(filename, tex, GL_LINEAR, GL_LINEAR);
}

SDL_Surface* create_texture_and_surface_from_file(const char* filename, GLuint* tex, GLuint min_filter, GLuint mag_filter)
{
    IF_ASSERT(tex == NULL) return NULL;
    *tex = 0;
    SDL_Surface* surface = IMG_Load(filename);
    IF_ASSERT(surface == NULL)
    {
        printf("Error loading texture %s, %s\n", filename, IMG_GetError());
        return NULL;
    }
    int ret = create_texture_from_surface(surface, tex, min_filter, mag_filter);
    IF_ASSERT(ret)
    {
        SDL_FreeSurface(surface);
        return NULL;
    }
    return surface;
}

SDL_Surface* create_surface_from_nothing(int w, int h)
{
    // taken from http://sdl.beuc.net/sdl.wiki/SDL_CreateRGBSurface
    SDL_Surface* surface = NULL;
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
    IF_ASSERT(surface == NULL)
        printf("CreateRGBSurface failed: %s\n", SDL_GetError());
    return surface;
}

void load_colored_texture(const char* path, GLuint* texture,
                          class Color base_color, class Color color)
{
    SDL_Surface* s = NULL;
    s = create_surface_from_file(path);
    IF_ASSERT(s == NULL) return;

    SDL_LockSurface(s);
    glEnable(GL_TEXTURE_2D);

    // alter surface, swapping base rgb with new rgb
    Uint32 pix;
    Uint8 sr,sg,sb,sa;
    for (int i=0; i<s->w*s->h; i++)
    {
        pix = ((Uint32*)s->pixels)[i];
        SDL_GetRGBA(pix, s->format, &sr, &sg, &sb, &sa);
        if (sr == base_color.r && sg == base_color.g && sb == base_color.b)
            ((Uint32*)s->pixels)[i] = SDL_MapRGBA(s->format, color.r, color.g, color.b, sa);
    }
    glDisable(GL_TEXTURE_2D);

    // create texture
    int ret = create_texture_from_surface(s, texture);
    IF_ASSERT(ret)
        printf("ERROR: failed to create texture from surface for %s\n", path);

    // cleanup
    SDL_UnlockSurface(s);
    SDL_FreeSurface(s);
}

void save_surface_to_png(SDL_Surface* surface, const char* filename)
{
    size_t png_size;
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
    IF_ASSERT(png_data == NULL) return;
    FILE* pFile = fopen(filename, "wb");
    IF_ASSERT(pFile == NULL)
    {
        free(png_data);
        return;
    }
    fwrite(png_data, 1, png_size, pFile);
    fclose(pFile);
    free(png_data);
}
