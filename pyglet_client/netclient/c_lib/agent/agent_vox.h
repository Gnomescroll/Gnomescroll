#ifndef objects_agent_vox_h
#define objects_agent_vox_h

#include <math.h>

//#include <ray_trace/ray_trace.h>
//#include <t_map/t_map.h>
//#include <t_map/t_properties.h>

#include "agent.h"
#include "vector.h"

void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize);
void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a);
void set_agent_box_anchor_point(int id, int part, float ax,float ay,float az, float fx,float fy,float fz);

struct Voxel {
unsigned char r,g,b,a;
};


struct Vox {
    //float ax,ay,az; //anchor
    struct Vector a;
    float length; //length from anchor to center
    float cx,cy,cz;

    struct Vector f,r,u;
    //float fx,fy,fz;
    //float rx,ry,rz;
    //float ux,uy,uz; //up, right, forward

    unsigned short xdim;
    unsigned short ydim;
    unsigned short zdim;
    float vox_size;
    float radius;
    struct Voxel* vox;
    unsigned int num_vox;
};

void destroy_vox(struct Vox* v);

//void agent_vox_draw_head(struct Vox* v, struct Vector look, struct Vector right, struct Agent_state* a);

#endif
