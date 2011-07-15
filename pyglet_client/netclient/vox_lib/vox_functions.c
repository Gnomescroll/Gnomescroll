#include "./vox_functions.h"


#define pi 3.14159
struct VoxelList* vo;

//util
inline float vlength(struct Vector v) {
    float length = 0;
    length = v.x*v.x + v.y*v.y + v.z*v.z;
    return length;
}

inline float iproduct(struct Vector v1, struct Vector v2) {
    float ip = 0;
    ip = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
    return ip;
}

struct Vertex {
    float x,y,z;
};


int v_set[3*8] = {
        0,0,0,
        1,0,0,
        1,1,0,
        0,1,0,
        0,0,1,
        1,0,1,
        1,1,1,
        0,1,1 };

/*
int q_set[4*6]= {
    4,5,6,7,   //top (z=1)
    0,1,2,3,   //bottom (z=0)
    1,5,9,10,  //north (y=1)
    7,3,11,8,  //south (y=0)
    6,2,10,11, //west (x=0)
    4,0,9,8   //east (x=1)
    };
*/

int q_set[4*6]= {
        4,5,6,7,
        3,2,1,0,
        2,3,7,6,
        0,1,5,4,
        0,4,7,3,
        1,2,6,5 };


//end util

inline void compute_vo_normals(struct VoxelList* volist) {
    //const struct Vector* center = &skel->center;
    float theta = volist->theta;
    //float phi = volist->phi;
    struct Vector* n = volist->n;

    n[0].x = volist->vosize*cos(theta);
    n[0].y = volist->vosize*sin(theta);
    n[0].z = 0;

    n[1].x = volist->vosize*cos(theta+ pi/2);
    n[1].y = volist->vosize*sin(theta+ pi/2);
    n[1].z = 0;

    n[2].x = 0;
    n[2].y = 0;
    n[2].z = volist->vosize*1;

/*
    printf("Normal x: %f, %f, %f \n", n[0].x, n[0].y, n[0].z);
    printf("Normal y: %f, %f, %f \n", n[1].x, n[1].y, n[1].z);
    printf("Normal z: %f, %f, %f \n", n[2].x, n[2].y, n[2].z);
    printf("Normal Length: %f, %f, %f \n", vlength(n[0]), vlength(n[1]), vlength(n[2]));

    printf("<x,y>: %f \n",  iproduct(n[0], n[1]));
    printf("<x,z>: %f \n",  iproduct(n[0], n[2]));
    printf("<y,z>: %f \n",  iproduct(n[1], n[2]));
*/
}


struct VoxelList* createVoxelList(int xdim, int ydim, int zdim) {
    struct VoxelList* volist = (struct VoxelList*) malloc(sizeof(struct VoxelList));
    volist->vosize = 0.2;
    volist->xdim = xdim;
    volist->ydim = ydim;
    volist->zdim = zdim;
    volist->center.x = 0;
    volist->center.y = 0;
    volist->center.z = 3;
    volist->list = (struct Voxel*) malloc(xdim*ydim*zdim*sizeof(struct Voxel));
    return volist;
}

inline struct Voxel get(struct VoxelList* vl, int x, int y, int z) {
    return vl->list[x+ y*vl->ydim + z*vl->xdim*vl->ydim];
}

inline void set(struct VoxelList* vl, int x, int y, int z, int r, int g, int b) {
    struct Voxel* t = &vl->list[x+ y*vl->ydim + z*vl->xdim*vl->ydim];
    t->r = r;
    t->g = g;
    t->b = b;
}

int init7() {
    vo = createVoxelList(8,8,8);
    //printf("No seg fault yet \n");
    set(vo, 0,0,0,255,0,0);
    set(vo, 7,7,7,0,255,0);
    set(vo, 4,4,4,255,0,255);
    set(vo, 3,3,3,255,0,255);
//    compute_vo_normals(vo);
}

int draw_vol(struct VoxelList* vl, struct Voxel voi, int xi, int yi, int zi) {
    struct Vertex vlist[8];
    //printf("t= %i \n", xi);

    if(voi.r == 0 && voi.g == 0 && voi.b == 0)
        return 0;
    int i,j;
    for(i=0; i<8; i++) {
        vlist[i].x = 0;
        vlist[i].y = 0;
        vlist[i].z = 0;
        if(v_set[3*i+0] == 1) {
            vlist[i].x += xi*vl->n[0].x;
            vlist[i].y += xi*vl->n[0].y;
            vlist[i].z += xi*vl->n[0].z;
        } else {
            vlist[i].x += (xi+1)*vl->n[0].x;
            vlist[i].y += (xi+1)*vl->n[0].y;
            vlist[i].z += (xi+1)*vl->n[0].z;
        }
        if(v_set[3*i+1] == 1) {
            vlist[i].x += yi*vl->n[1].x;
            vlist[i].y += yi*vl->n[1].y;
            vlist[i].z += yi*vl->n[1].z;
        } else {
            vlist[i].x += (yi+1)*vl->n[1].x;
            vlist[i].y += (yi+1)*vl->n[1].y;
            vlist[i].z += (yi+1)*vl->n[1].z;
        }
        if(v_set[3*i+2] == 1) {
            vlist[i].x += zi*vl->n[2].x;
            vlist[i].y += zi*vl->n[2].y;
            vlist[i].z += zi*vl->n[2].z;
        } else {
            vlist[i].x += (zi+1)*vl->n[2].x;
            vlist[i].y += (zi+1)*vl->n[2].y;
            vlist[i].z += (zi+1)*vl->n[2].z;
        }

        vlist[i].x += vl->center.x;
        vlist[i].y += vl->center.y;
        vlist[i].z += vl->center.z;

        //printf("z= %f \n", vl->center.z);
        //printf("Vertex: %f, %f, %f \n", vlist[i].x, vlist[i].y, vlist[i].z);
    }
    struct Vertex* vt;

    for(i=0; i<6;i++) {
                glColor3ub(voi.r, voi.g, voi.b);

                glVertex3f(vlist[q_set[4*i+0]].x,vlist[q_set[4*i+0]].y,vlist[q_set[4*i+0]].z);
                //glTexCoord2i( 1, 0 );
                glVertex3f(vlist[q_set[4*i+1]].x,vlist[q_set[4*i+1]].y,vlist[q_set[4*i+1]].z);
                //glTexCoord2i( 1, 1 );
                glVertex3f(vlist[q_set[4*i+2]].x,vlist[q_set[4*i+2]].y,vlist[q_set[4*i+2]].z);
                //glTexCoord2i( 0, 1 );
                glVertex3f(vlist[q_set[4*i+3]].x,vlist[q_set[4*i+3]].y,vlist[q_set[4*i+3]].z);
                vt = &vlist[q_set[4*i+j]];
    }

    return 0;
}


int _draw_test2() {
    int xi, yi, zi;
    vo->theta += pi/1024;
    compute_vo_normals(vo);

    struct Voxel voi;
    glEnable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    for(xi = -vo->xdim/2; xi < vo->xdim/2;xi++ ) {
        for(yi = -vo->ydim/2; yi < vo->ydim/2; yi++ ) {
            for(zi = -vo->zdim/2; zi < vo->zdim/2; zi++) {
                //aprintf("t= %i %i %i\n", xi,yi,zi);
                voi = get(vo, xi+vo->xdim/2, yi+vo->ydim/2, zi+vo->zdim/2);
                draw_vol(vo, voi, xi,yi,zi);

            }
        }
    }
    glEnd();
    //glDisable(GL_DEPTH_TEST);
    glColor3ub(255,255,255);
    return 0;
}

//external interface functions
int _draw(struct VoxelList* vo) {
    int xi, yi, zi;
    struct Voxel voi;
    compute_vo_normals(vo);
    glEnable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    for(xi = -vo->xdim/2; xi < vo->xdim/2;xi++ ) {
        for(yi = -vo->ydim/2; yi < vo->ydim/2; yi++ ) {
            for(zi = -vo->zdim/2; zi < vo->zdim/2; zi++) {
                //aprintf("t= %i %i %i\n", xi,yi,zi);
                voi = get(vo, xi+vo->xdim/2, yi+vo->ydim/2, zi+vo->zdim/2);
                draw_vol(vo, voi, xi,yi,zi);

            }
        }
    }
    glEnd();
    //glDisable(GL_DEPTH_TEST);
    glColor3ub(255,255,255);
    return 0;
}

inline struct Voxel _get(struct VoxelList* vl, int x, int y, int z) {
    return vl->list[x+ y*vl->ydim + z*vl->xdim*vl->ydim];
}

inline void _set(struct VoxelList* vl, int x, int y, int z, int r, int g, int b, int a) {
    struct Voxel* t = &vl->list[x+ y*vl->ydim + z*vl->xdim*vl->ydim];
    t->r = r;
    t->g = g;
    t->b = b;
    t->a = a;
}

struct VoxelList* _createVoxelList(float vo_size, int xdim, int ydim, int zdim, float x, float y, float z, float theta) {
    struct VoxelList* volist = (struct VoxelList*) malloc(sizeof(struct VoxelList));
    volist->vosize = vo_size;
    volist->xdim = xdim;
    volist->ydim = ydim;
    volist->zdim = zdim;
    volist->center.x = x;
    volist->center.y = y;
    volist->center.z = z;
    volist->list = (struct Voxel*) malloc(xdim*ydim*zdim*sizeof(struct Voxel));
    int i;
    struct Voxel* v;
    for(i=0;i<xdim*ydim*zdim;i++) {
        v = &volist->list[i];
        v->r = 0;
        v->g = 0;
        v->b = 0;
        v->a = 0;
    }
    return volist;
}

int _deleteVoxelList(struct VoxelList* vo) {
    free(vo);
    return 0;
}

int _point_collision(struct VoxelList* vo, float x, float y, float z) {
    x -= vo->center.x;
    y -= vo->center.y;
    z -= vo->center.z;
    struct Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    float a,b,c;
    a = iproduct(v, vo->n[0]);
    if(a < -vo->xdim || a > vo->xdim) return 0;
    b = iproduct(v, vo->n[1]);
    if(b < -vo->ydim || b > vo->ydim) return 0;
    c = iproduct(v, vo->n[2]);
    if(c < -vo->zdim || c > vo->zdim) return 0;

    int _x, _y, _z;
    _x = (int) a + vo->xdim/2;
    _y = (int) b + vo->ydim/2;
    _z = (int) c + vo->zdim/2;
    struct Voxel voi = get(vo,x,y,z);
    if(voi.r == 0 && voi.g == 0 && voi.r == 0) return 0;
    return 1;
}
