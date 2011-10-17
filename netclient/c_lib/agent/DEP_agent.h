#pragma once

#include <math.h>
#include <compat.h>

#include <agent/agent_vox.hpp>

#define AGENT_PART_NUM 6
#define AGENT_PART_HEAD 0
#define AGENT_PART_TORSO 1
#define AGENT_PART_LARM 2
#define AGENT_PART_RARM 3
#define AGENT_PART_LLEG 4
#define AGENT_PART_RLEG 5

#define AGENT_LIST2_SIZE 1024

struct Agent_vox_old {
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

struct Agent_vox_old** Agent_list2 = NULL;

void agent_tick_vox();
int create_agent_vox(float x, float y, float z);
//void set_agent_state(int id, float xangle, float yangle);

/*
 *  Client only
 */
#ifdef DC_CLIENT
void agent_draw_vox();
#endif

//struct Agent_vox_old* get_agent_vox(int id);
struct Vox* get_agent_vox_part(int id, int part);

void init_agent_vox_module();
void shutdown_agent_vox_module();
