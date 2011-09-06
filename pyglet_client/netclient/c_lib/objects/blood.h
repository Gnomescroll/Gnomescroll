#ifndef objects_blood_h
#define objects_blood_h

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

void init_objects_blood();
void blood_tick();
void create_blood(int type, float x, float y, float z, float vx, float vy, float vz);

//#ifdef DC_CLIENT
void blood_draw();
//#endif

#endif
