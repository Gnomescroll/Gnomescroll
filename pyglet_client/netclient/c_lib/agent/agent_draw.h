#ifndef agent_agent_draw_h
#define agent_agent_draw_h

#include <math.h>

#include <compat.h>
#include <compat_gl.h>

#include "vector.h"


void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle);
void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height);
void draw_agent_box_selection(int x, int y, int z, int r, int g, int b);

#endif
