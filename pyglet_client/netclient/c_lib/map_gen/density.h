#ifndef objects_agent_vox_h
#define objects_agent_vox_h


#define NO_SDL_GLEXT
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"


#include <math.h>

#include <compat.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

void init_map_gen_density();
void compute_density_map();
void map_density_visualize(int density, float min, float max);

#endif
