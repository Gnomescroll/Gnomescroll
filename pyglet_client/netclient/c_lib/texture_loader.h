#ifndef texture_loader_h
#define texture_loader_h

#define NO_SDL_GLEXT
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_image.h> //temp?

#include "compat.h"

int init_texture_loader();

int get_particle_texture();


#endif
