/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

//#include <common/compat_gl.h>

struct Texture
{
    GLuint tex;
    int w;
    int h;
};

int init_image_loader();

GLenum get_texture_format(SDL_Surface* surface);

SDL_Surface* _load_image(const char *file);
void _load_image_create_texture(const char *file, struct Texture* tex);

SDL_Surface* create_surface_from_file(const char* file);
int create_texture_from_file(const char* filename, GLuint* tex);
int create_texture_from_file(const char* filename, GLuint* tex, GLuint min_filter, GLuint mag_filter);

int create_texture_from_surface(SDL_Surface* surface, GLuint* tex);
int create_texture_from_surface(SDL_Surface *surface, GLuint *tex, GLuint mag_filter);
int create_texture_from_surface(SDL_Surface *surface, GLuint *tex, GLuint min_filter, GLuint mag_filter);

SDL_Surface* create_texture_and_surface_from_file(const char* filename, GLuint* tex);
SDL_Surface* create_texture_and_surface_from_file(const char* filename, GLuint* tex, GLuint min_filter, GLuint mag_filter);

SDL_Surface* create_surface_from_nothing(int w, int h);

void load_colored_texture(const char* path, GLuint* texture,
                          class Color base_color, class Color color);

void save_surface_to_png(SDL_Surface* surface, const char* filename);
