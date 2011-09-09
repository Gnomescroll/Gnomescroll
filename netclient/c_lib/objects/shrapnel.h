#ifndef objects_shrapnel_h
#define objects_shrapnel_h

#include <compat.h>
#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

//#include <c_lib.h>
#include <ray_trace/ray_trace.h>
//#include <t_map/t_properties.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#ifdef DC_CLIENT
#include <texture_loader.h>
#endif

void init_objects_shrapnel();
void shrapnel_tick();
void create_shrapnel(int type, float x, float y, float z, float vx, float vy, float vz);

#ifdef DC_CLIENT
void shrapnel_draw();
#endif

#endif
