#ifndef objects_agent_vox_h
#define objects_agent_vox_h

#include <math.h>

//#include <ray_trace/ray_trace.h>
//#include <t_map/t_map.h>
//#include <t_map/t_properties.h>

#include "agent.h"

void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize);
void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a);

struct Voxel {
unsigned char r,g,b,a;
};

struct Vox {
    float ax,ay,az; //anchor
    float length; //length from anchor to center
    float cx,cy,cz;

    float fx,fy,fz;
    float rx,ry,rz;
    float ux,uy,uz; //up, right, forward

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
