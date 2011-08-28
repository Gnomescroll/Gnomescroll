#ifndef map_get_density_h
#define map_get_density_h


#define NO_SDL_GLEXT
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"


#include <math.h>

#include <compat.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>


struct Map_density_element {
    float density;

};

void init_map_gen_density();
void update_density_map(int iterations);
void map_density_visualize(int density, float min, float max);

#endif
