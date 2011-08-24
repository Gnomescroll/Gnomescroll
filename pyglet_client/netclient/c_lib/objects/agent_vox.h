#ifndef objects_agent_vox_h
#define objects_agent_vox_h

#include <math.h>

//#include <ray_trace/ray_trace.h>
//#include <t_map/t_map.h>
//#include <t_map/t_properties.h>

void agent_tick();
void agent_draw();
int create_agent(float x, float y, float z);
void set_agent_state(int id, float xangle, float yangle);

struct Voxel {
unsigned char r,g,b,a;
};

struct Vox {
    unsigned short xdim;
    unsigned short ydim;
    unsigned short zdim;
    float vox_size;
    float radius;
    struct Voxel* vox
};

struct Agent_vox {
struct Vox head,torso;
struct Vox larm,rarm;
struct Vox lleg,rleg,
};

#endif
