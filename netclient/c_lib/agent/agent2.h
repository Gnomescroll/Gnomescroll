#include "net_agent.c"

struct Agent_cs { //control state
    int tick;
    int seq;
    float theta,phi;
    uint32_t cs; //movement, jetpack, as flag
};

struct Agent_snapshot {
    int id; //ignore on client
    int tick;
    float x,y,z;
    float vx,vy,vz;
    //float theta,phi;
}
