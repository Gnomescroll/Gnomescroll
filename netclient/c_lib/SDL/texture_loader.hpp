#pragma once

#include <compat_gl.h>

struct Texture {
    int tex;
    int w;
    int h;
};

int init_image_loader();

SDL_Surface* _load_image(char *file);
struct Texture _load_image_create_texture(char *file);
void _load_image_create_texture(char *file, struct Texture* tex);

SDL_Surface* create_surface_from_file(char* file);
int create_texture_from_file(char* filename, int* tex);   // deprecated
int create_texture_from_file(char* filename, GLuint* tex);
int create_texture_from_surface(SDL_Surface* surface, int* tex);   // deprecated
int create_texture_from_surface(SDL_Surface* surface, GLuint* tex);
SDL_Surface* create_texture_and_surface_from_file(char* filename, GLuint* tex);

SDL_Surface* create_surface_from_nothing(int w, int h);

///* http://content.gpwiki.org/index.php/SDL:Tutorials:Displaying_a_Bitmap_from_a_Custom_Resource_File_using_SDL_RWops */
//SDL_Surface* create_surface_from_bitmap(unsigned char *bmp, int size);
