#ifndef objects_neutron_h
#define objects_neutron_h


#define NO_SDL_GLEXT
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"

//#include <c_lib.h>
#include <ray_trace/ray_trace.h>
//#include <t_map/t_properties.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#include "../texture_loader.h"

void init_objects_neutron();
void neutron_tick();
void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz);
void neutron_draw();

#endif
