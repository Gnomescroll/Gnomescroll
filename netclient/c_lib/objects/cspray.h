#ifndef objects_cspray_h
#define objects_cspray_h

#include <compat.h>
#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <physics/common.h>
#include <ray_trace/ray_trace.h>
#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#ifdef DC_CLIENT
#include <texture_loader.h>
#endif

void init_objects_cspray();
void cspray_tick();
void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz);

/*
 *  Client only
 */
#ifdef DC_CLIENT
void cspray_draw();
#endif

#endif
