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
    v->vox_size = vosize;
    v->vox = (struct Voxel *) malloc (v->num_vox*sizeof(struct Voxel));
    int i;
    for(i=0; i<v->num_vox; i++) {
        v->vox[i].r = 0;
        v->vox[i].g = 0;
        v->vox[i].b = 255;
        v->vox[i].a = 0; //if alpha is zero, dont draw
    }
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

void print_vector(struct Vector * v) {
    printf("%f, %f, %f \n", v->x, v->y, v->z);
}
void agent_vox_draw_head(struct Vox* v, struct Vector look, struct Vector right, struct Agent_state* a) {
    //printf("draw head\n");

    float ch = a->camera_height;
    //look is forward direction
    //right is right
    float vos = v->vox_size;
    //printf("vos= %f \n", vos);

    struct Vector c = Vector_init(a->x, a->y, a->z + ch);

    struct Vector vx,vy,vz;

    vx = look;
    vz = vector_cross(vx, right);
    vy = vector_cross(vx, vz);

    //print_vector(&right);
    //print_vector(&vx); print_vector(&vy); print_vector(&vz);
    //print_vector_dot(vx, vz); print_vector_dot(vx, vy); print_vector_dot(vy, vz);

    struct Voxel* vo;

    float x0, y0, z0;
    int i,j,k;
    glBegin(GL_POINTS);
    for(i= -v->xdim/2; i < v->xdim/2; i++) {
    for(j= -v->ydim/2; j < v->ydim/2; j++) {
    for(k= -v->zdim/2; k < v->zdim/2; k++) {
    vo = &v->vox[(i+v->xdim/2) + (j+v->ydim/2)*v->ydim + ((k+v->zdim/2))*v->zdim*v->ydim];
    if(vo->a == 0) continue;
    glColor3ub((unsigned char)vo->r,(unsigned char)vo->g,(unsigned char)vo->b);
/*
    x0 = c.x + vos*(_i*vx.x + _j*vy.x + _k*vz.x);
    y0 = c.y + vos*(_i*vx.y + _j*vy.y + _k*vz.y);
    z0 = c.z + vos*(_i*vx.z + _j*vy.z + _k*vz.z);
*/
    x0 = c.x + vos*(i*vx.x + j*vy.x + k*vz.x);
    y0 = c.y + vos*(i*vx.y + j*vy.y + k*vz.y);
    z0 = c.z + vos*(i*vx.z + j*vy.z + k*vz.z);

    glVertex3f(x0,y0,z0); // point
    //printf("%i, %i, %i \n", i,j,k);
    //printf("%f, %f, %f \n", x0, y0, z0);
    }
    }
    }
    glEnd();

}
