/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <math.h>
#include <common/compat_gl.h>

#include <agent/agent.hpp>

namespace AgentDraw
{

void draw_agent(Agents::Agent* g);
void draw_agent_aiming_direction(float x, float y, float z, float theta, float phi);
void draw_agent_bounding_box(float x, float y, float z, float radius,
                             float head_height, float height);
void draw_agent_bounding_box(float x, float y, float z, float radius,
                             float head_height, float height, Color color);
void draw_agent_cube_selection(const Vec3i& position, Color color);

}   // AgentDraw
