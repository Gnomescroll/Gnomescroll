#ifndef objects_agent_vox_h
#define objects_agent_vox_h

#include <math.h>

//#include <ray_trace/ray_trace.h>
//#include <t_map/t_map.h>
//#include <t_map/t_properties.h>

#include "agent.h"

void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize);

struct Voxel {
unsigned char r,g,b,a;
};

struct Vox {
    float cx,cy,cz;

    float ux,uy,uz; //up, right, forward
    float rx,ry,rz;
    float fx,fy,fz;

    unsigned short xdim;
    unsigned short ydim;
    unsigned short zdim;
    float vox_size;
    float radius;
    struct Voxel* vox;
    unsigned int num_vox;
};

void destroy_vox(struct Vox* v);

#endif
