#include "agent_vox.h"
#include "agent.h"

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

//

//agent_vox_draw_head(struct Vox* v, struct Vector look, struct Vector right, struct Agent_state a);

void agent_vox_draw_head(struct Vox* v, struct Vector look, struct Vector right, struct Agent_state* a) {
/*
    float ch = a->camera_height
    //look is forward direction
    //right is right
    float vos = v->vox_size;

    struct Vector c = Vector_init(a->x, a->y, ch*z);

    struct Vector vx,vy,vz;

    vx = look;
    vz = vector_cross(vx, right)
    vy = vector_cross(vx, vz)

    struct Voxel* vo;
    int i,j,k;
    glBegin(GL_POINTS);
    for(i= -v->xdim/2; i < v->xdim/2; i++) {
    for(j= -v->ydim/2; j < v->ydim/2; j++) {
    for(k= -v->zdim/2; k < v->zdim/2; k++) {
    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);

    glVertex3f(x0,y0,z0); // point

    }}}
    glEnd();
*/
}
