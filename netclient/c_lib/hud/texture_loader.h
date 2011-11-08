#pragma once

#include <compat_gl.h>

typedef struct {
    int tex;
    int w;
    int h;
} Texture;

int _init_image_loader();
SDL_Surface* _load_image(char *file);

int _create_block_texture(char *file);

void _draw_loaded_hud_texture(int x, int y);
void _load_hud_texture(char *file);

int init_texture_from_file(char* filename, int* tex);
int init_texture_from_surface(SDL_Surface* surface, int* tex);
