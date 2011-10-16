#pragma once

#include <math.h>

#include <compat.h>

#include <ray_trace/ray_trace.h>

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

//#include "agent_vox.h"
//#include "agent_draw.h"

#include <physics/vector.h>

#define AGENT_PART_NUM 6
#define AGENT_PART_HEAD 0
#define AGENT_PART_TORSO 1
#define AGENT_PART_LARM 2
#define AGENT_PART_RARM 3
#define AGENT_PART_LLEG 4
#define AGENT_PART_RLEG 5

struct Agent_vox {
int id;
float x,y,z;
float vx,vy,vz;
float xangle, yangle;
float lv,ly,lz; //looking vector
float camera_height;
float cbox_height;
float cbox_radius; // collision box

struct Vox vox_part[AGENT_PART_NUM]; //head,torso, larm,rarm, lleg, rleg
};

void agent_tick_vox();
int create_agent_vox(float x, float y, float z);
//void set_agent_state(int id, float xangle, float yangle);

/*
 *  Client only
 */
#ifdef DC_CLIENT
void agent_draw_vox();
#endif

//struct Agent_vox* get_agent_vox(int id);
struct Vox* get_agent_vox_part(int id, int part);
