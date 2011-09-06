#ifndef objects_neutron_h
#define objects_neutron_h

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

void init_objects_neutron();
void neutron_tick();
void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz);

//#ifdef DC_CLIENT
void neutron_draw();
//#endif

#endif
