#ifndef objects_agent_h
#define objects_agent_h

#include <math.h>

#include <ray_trace/ray_trace.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#include "agent_vox.h"

#include "vector.h"

void agent_tick();
void agent_draw();
int create_agent(float x, float y, float z);
void set_agent_state(int id, float xangle, float yangle);

#define AGENT_PART_NUM 6
#define AGENT_PART_HEAD 0
#define AGENT_PART_TORSO 1
#define AGENT_PART_LARM 2
#define AGENT_PART_RARM 3
#define AGENT_PART_LLEG 4
#define AGENT_PART_RLEG 5

struct Agent_state {
int id;
float x,y,z;
float vx,vy,vz;
float xangle, yangle;
float lv,ly,lz; //looking vector
float camera_height;
float cbox_height;
float cbox_radius; // collision box

struct Vox vox_part[6]; //head,torso, larm,rarm, lleg, rleg
};

struct Agent_state* get_agent(int id);
struct Vox* get_agent_vox_part(int id, int part);
#endif
