#ifndef block_selector_h
#define block_selector_h

#define NO_SDL_GLEXT

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "math.h"


int _init_block_selector();

void _draw_block_selector(int x, int y);
void _load_block_selector_texture(char *file, int scale);
int _blit_sprite2(int tex, float x0, float y0, float x1, float y1, float z);

#endif
