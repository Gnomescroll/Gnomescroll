#ifndef objects_agent_h
#define objects_agent_h

#include <math.h>

#include <ray_trace/ray_trace.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

void agent_tick();
void agent_draw();
int create_agent(float x, float y, float z);
void set_agent_state(int id, float xangle, float yangle);

struct Agent_state {
int id;
float x,y,z;
float vx,vy,vz;
float xangle, yangle;
float lv,ly,lz; //looking vector
float camera_height;
float cbox_height;
float cbox_radius; // collision box
};

#endif
