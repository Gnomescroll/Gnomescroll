#ifndef objects_agent_vox_h
#define objects_agent_vox_h

#include <math.h>

#include <compat.h>
#include <compat_gl.h>

//#include <ray_trace/ray_trace.h>
//#include <t_map/t_map.h>
//#include <t_map/t_properties.h>

#include "vector.h"
//#include "agent.h"
struct Agent_state;

struct Voxel {
unsigned char r,g,b,a;
};


struct Vox {

    struct Vector f;
    struct Vector r,u;
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


    struct Vector a; //anchor
    struct Vector c; //center
    float length;
};

void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize);

void set_limb_properties(int id, int part, float length, float ax, float ay, float az);
void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a);
void set_agent_box_anchor_point(int id, int part, float fx,float fy,float fz);


void destroy_vox(struct Vox* v);
void agent_vox_draw_head(struct Vox* v, struct Vector look, struct Vector right, struct Agent_state* a);
void agent_vox_draw_vox_volume(struct Vox* v, struct Vector right, struct Agent_state* a);
#endif
