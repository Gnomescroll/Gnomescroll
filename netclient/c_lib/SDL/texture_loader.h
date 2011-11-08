#pragma once

#include <compat_gl.h>

typedef struct {
    int tex;
    int w;
    int h;
} Texture;

int init_image_loader();

SDL_Surface* _load_image(char *file);

int create_texture_from_file(char* filename, int* tex);
int create_texture_from_surface(SDL_Surface* surface, int* tex);
