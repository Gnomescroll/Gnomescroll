#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <math.h>
#include <common/compat_gl.h>

#include <agent/agent.hpp>
#include <agent/agent_vox.hpp>

namespace AgentDraw
{

void add_snapshot_to_history(Agent_state* g);

void draw_agents(Agent_list* agent_list); 
void draw_agent(Agent_state* g); 

void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle);
void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height);
void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height, int r, int g, int b) ;
void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b);
void draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b);

}
