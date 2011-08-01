#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "camera.h"

int _init_particle_functions();

int _draw_particle(int id, float size, float x, float y, float z);
int _draw_particle2(int id, float size, float x, float y, float z);
