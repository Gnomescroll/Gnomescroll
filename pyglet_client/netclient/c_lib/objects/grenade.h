#ifndef objects_grenade_h
#define objects_grenade_h

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

void init_objects_grenade();
void grenade_tick();
int create_grenade(int type, float x, float y, float z, float vx, float vy, float vz, uint ttl, uint ttl_max);
void destroy_grenade(int gid);

#ifdef DC_CLIENT
static inline void grenade_draw();
#endif

#endif
