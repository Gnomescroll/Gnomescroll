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


int v_set[3*8] = {
        0,0,0,
        1,0,0,
        1,1,0,
        0,1,0,
        0,0,1,
        1,0,1,
        1,1,1,
        0,1,1 };

int q_set[4*6]= {
        4,5,6,7,
        3,2,1,0,
        2,3,7,6,
        0,1,5,4,
        0,4,7,3,
        1,2,6,5 };

float v_buffer[3*8];
float s_buffer[6*(4*3)];

void print_vector(struct Vector * v) {
    printf("%f, %f, %f \n", v->x, v->y, v->z);
}
/*
void t_draw_cube() {
    glColor3ub(0,0,255);
    int i;
    int j;
    float x,y,z;
    x=2;y=2;z=8;
    glBegin(GL_QUADS);
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++){
            glVertex3f(x+ s_buffer[12*i+3*j+0], y+ s_buffer[12*i+3*j+1], z+ s_buffer[12*i+3*j+2]);
        }
    }
    glEnd();
}
*/
//void agent_vox_draw_head(struct Vox* v, struct Vector look, struct Vector right, struct Agent_state* a) {
    //float ch = a->camera_height;
    ////look is forward direction
    ////right is right
    //float vos = v->vox_size;

    //struct Vector c = Vector_init(a->x, a->y, a->z + ch);

    //struct Vector vx,vy,vz;

    //vx = look;
    //vz = vector_cross(vx, right);
    //vy = vector_cross(vx, vz);
    //int i,j,k;

    //for(i=0; i<8; i++) {
        //v_buffer[3*i+0] = vos*(v_set[3*i+0]*vx.x + v_set[3*i+1]*vy.x + v_set[3*i+2]*vz.x );
        //v_buffer[3*i+1] = vos*(v_set[3*i+0]*vx.y + v_set[3*i+1]*vy.y + v_set[3*i+2]*vz.y );
        //v_buffer[3*i+2] = vos*(v_set[3*i+0]*vx.z + v_set[3*i+1]*vy.z + v_set[3*i+2]*vz.z );
    //}
    //for(i=0; i<6; i++) {
        //for(j=0; j<4; j++) {
            //s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            //s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            //s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        //}
    //}

    //struct Voxel* vo;

    //float x0, y0, z0;

    //int i1, j1;
    ////glBegin(GL_POINTS);
    //glBegin(GL_QUADS);
    //for(i= -v->xdim/2; i < v->xdim/2; i++) {
    //for(j= -v->ydim/2; j < v->ydim/2; j++) {
    //for(k= -v->zdim/2; k < v->zdim/2; k++) {
    //vo = &v->vox[(i+v->xdim/2) + (j+v->ydim/2)*v->ydim + ((k+v->zdim/2))*v->zdim*v->ydim];
    //if(vo->a == 0) continue;
    //glColor3ub((unsigned char)vo->r,(unsigned char)vo->g,(unsigned char)vo->b);

    //x0 = c.x + vos*(i*vx.x + j*vy.x + k*vz.x);
    //y0 = c.y + vos*(i*vx.y + j*vy.y + k*vz.y);
    //z0 = c.z + vos*(i*vx.z + j*vy.z + k*vz.z);

    ////printf("%f, %f, %f \n", x0,y0,z0);
    ////printf("%i,%i,%i \n", i,j,k);


    ////glBegin(GL_POINTS);
    ////    glVertex3f(x0,y0,z0);
    ////glEnd();

    //for(i1=0; i1<6; i1++) {
        //for(j1=0; j1<4; j1++){
            ////glVertex3f(x0+ s_buffer[12*i1+3*j1+0], y0+ s_buffer[12*i1+3*j1+1], z0+ s_buffer[12*i1+3*j1+2]);
        //}
            //glVertex3f(x0 + s_buffer[12*i1+3*0+0], y0+ s_buffer[12*i1+3*0+1], z0+ s_buffer[12*i1+3*0+2]);
            //glVertex3f(x0 + s_buffer[12*i1+3*1+0], y0+ s_buffer[12*i1+3*1+1], z0+ s_buffer[12*i1+3*1+2]);
            //glVertex3f(x0 + s_buffer[12*i1+3*2+0], y0+ s_buffer[12*i1+3*2+1], z0+ s_buffer[12*i1+3*2+2]);
            //glVertex3f(x0 + s_buffer[12*i1+3*3+0], y0+ s_buffer[12*i1+3*3+1], z0+ s_buffer[12*i1+3*3+2]);
    //}

///*
    //for(_i=0; _i<6; _i++) {
        //for(_j=0; _j<4; _j++){

        ////glBegin(GL_QUADS);
        ////    glVertex3f(x0 + s_buffer[12*_i+3*_j+0], y0+ s_buffer[12*_i+3*_j+1], z0+ s_buffer[12*_i+3*_j+2]);
        ////glEnd();

            //glVertex3f(x0 + s_buffer[12*_i+3*0+0], y0+ s_buffer[12*_i+3*0+1], z0+ s_buffer[12*_i+3*0+2]);
            //glVertex3f(x0 + s_buffer[12*_i+3*1+0], y0+ s_buffer[12*_i+3*1+1], z0+ s_buffer[12*_i+3*1+2]);
            //glVertex3f(x0 + s_buffer[12*_i+3*2+0], y0+ s_buffer[12*_i+3*2+1], z0+ s_buffer[12*_i+3*2+2]);
            //glVertex3f(x0 + s_buffer[12*_i+3*3+0], y0+ s_buffer[12*_i+3*3+1], z0+ s_buffer[12*_di+3*3+2]);

        //}
    //}
//*/
    ////glVertex3f(x0,y0,z0);

    //}}}
    //glEnd();

//}
