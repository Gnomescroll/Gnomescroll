#include "agent_vox.h"

void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize) {

/*
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
*/
    struct Vox* v = get_agent_vox_part(id, part);
    if(v == NULL) {
        printf("init_agent_vox_volume: Vox is Null!\n");
        return;
    }
    v->xdim = xdim;
    v->ydim = ydim;
    v->zdim = zdim;
    v->num_vox = xdim*ydim*zdim;
    v->radius = sqrt((vosize*xdim)*(vosize*xdim) + (vosize*ydim)*(vosize*ydim) + (vosize*zdim)*(vosize*zdim));
    v->vox = (struct Voxel *) malloc (v->num_vox*sizeof(struct Voxel));
}

void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a) {
    struct Vox* v = get_agent_vox_part(id, part);
    struct Voxel* vo = &v->vox[x + y*v->ydim + z*v->zdim*v->ydim];
    if(v == NULL || vo == NULL) {
        printf("set_agent_vox_volume: null pointer \n");
        return;
    }
    vo->r =r;
    vo->g =g;
    vo->b =b;
    vo->a =a;
    //x+ y*vl->ydim + z*vl->xdim*vl->ydim

}

void set_agent_box_anchor_point(int id, int part, float ax,float ay,float az, float fx,float fy,float fz) {
    struct Vox* v = get_agent_vox_part(id, part);
    if(v == NULL) {
        printf("set_agent_box_anchor_point: Vox is Null!\n");
        return;
    }
    v->a.x = ax;
    v->a.y = ay;
    v->a.z = az;

    float len = sqrt(fx*fx+fy*fy+fz*fz);
    fx /= len; fy /= len; fz /= len;

    v->f.x = fx;
    v->f.y = fy;
    v->f.z = fz;
}

void destroy_vox(struct Vox* v) {
    if(v->vox != NULL) free(v->vox);
}
