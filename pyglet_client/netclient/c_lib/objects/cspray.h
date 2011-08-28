#ifndef objects_cspray_h
#define objects_cspray_h

#include <compat.h>
#include <compat_gl.h>

//#include <c_lib.h>
#include <ray_trace/ray_trace.h>
//#include <t_map/t_properties.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#include <texture_loader.h>

void init_objects_cspray();
void cspray_tick();
void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz);
void cspray_draw();

#endif
