#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "camera.h"

#include "math.h"

int _init_particle_functions();

int _draw_particle(int id, float size, float x, float y, float z);
int _draw_particle2(int id, float size, float x, float y, float z);

int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1);
int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1);
