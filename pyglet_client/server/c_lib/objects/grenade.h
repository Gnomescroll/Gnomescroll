#ifndef objects_grenade_h
#define objects_grenade_h

//#include <compat.h>
//#include <compat_gl.h>

//#include <c_lib.h>
#include <ray_trace/ray_trace.h>
//#include <t_map/t_properties.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

//#include <texture_loader.h>

void init_objects_grenade();
void grenade_tick();
int create_grenade(int type, float x, float y, float z, float vx, float vy, float vz, uint ttl, uint ttl_max);
void destroy_grenade(int gid);
void grenade_draw();

#endif
