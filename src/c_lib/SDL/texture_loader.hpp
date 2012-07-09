#pragma once

//#include <common/compat_gl.h>

struct Texture
{
    GLuint tex;
    int w;
    int h;
};

int init_image_loader();

SDL_Surface* _load_image(char *file);
void _load_image_create_texture(char *file, struct Texture* tex);

SDL_Surface* create_surface_from_file(char* file);
int create_texture_from_file(char* filename, GLuint* tex);
int create_texture_from_file(char* filename, GLuint* tex, GLuint min_filter, GLuint mag_filter);

int create_texture_from_surface(SDL_Surface* surface, GLuint* tex);
int create_texture_from_surface(SDL_Surface *surface, GLuint *tex, GLuint MAG_FILTER);

SDL_Surface* create_texture_and_surface_from_file(char* filename, GLuint* tex);

SDL_Surface* create_surface_from_nothing(int w, int h);

void load_colored_texture(
    char* path, GLuint* texture,
    unsigned char br, unsigned char bg, unsigned char bb,   // base color
    unsigned char r, unsigned char g, unsigned char b       // replace with
);

void save_surface_to_png(SDL_Surface* surface, char* filename);
