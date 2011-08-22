#ifndef c_lib_h
#define c_lib_h

#define NO_SDL_GLEXT
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_image.h> //temp?


int init_c_lib();

extern GLint particle_sheet_id;

#endif
