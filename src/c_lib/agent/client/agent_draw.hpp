#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <math.h>
#include <common/compat_gl.h>

#include <agent/agent.hpp>

namespace AgentDraw
{

void add_snapshot_to_history(Agent* g);

void draw_agents(AgentList* agent_list); 
void draw_agent(Agent* g); 

void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle);
void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height);
void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height, unsigned char r, unsigned char g, unsigned char b);
void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b);

}   // AgentDraw
