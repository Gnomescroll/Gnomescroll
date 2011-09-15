#ifndef objects_agent_h
#define objects_agent_h

#include <stdint.h>

struct Agent_cs { //control state
    int tick;
    int seq;
    float theta, phi;
    uint32_t cs; //movement, jetpack, as flag
};

struct Agent_snapshot {
    //int id; //ignore on client
    int tick;
    float x,y,z;
    float vx,vy,vz;
    //float theta,phi;
};

struct Agent_state { //Agent_state
    int id;
    int local;

    float x,y,z;
    float vx,vy,vz;
    float theta,phi;

    int tick; //increment every control state change

    int ltick; //last tick/highest tick

    int tbn;
    struct Agent_cs t_buffer[128];

    struct Agent_snapshot last_snapshot;
    //collision box
    float camera_height;
    float cbox_height;
    float cbox_radius;
};

extern struct Agent_state* Agent_list[1024];
extern int agent_id_counter;
extern int a_count;

#include "net_agent.h"
#include "./agent_list.h"

    #ifdef DC_SERVER
    int create_agent(float x, float y, float z);
    #endif


    #ifdef DC_CLIENT
    struct Agent_state* create_agent_from_snapshot(int id, struct Agent_snapshot* as);
    #endif

#endif
