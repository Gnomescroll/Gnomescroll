#include "./skeleton_functions.h"


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
//end util

inline void compute_vo_normals(struct VoxelList* volist) {
    //const struct Vector* center = &skel->center;
    float theta = volist->theta;
    //float phi = volist->phi;
    struct Vector* n = volist->n;

    float theta_90 = skel->theta+ pi/2;
    float phi_90 = skel->phi+ pi/2;

    n[0].x = volist->vosize*cos(theta)
    n[0].y = volist->vosize*sin(theta)
    n[0].z = 0

    n[1].x = volist->vosize*cos(theta+ pi/2);
    n[1].y = volist->vosize*sin(theta+ pi/2);
    n[1].z = 0

    n[2].x = 0
    n[2].y = 0
    n[2].z = volist->vosize*1;

    printf("Normal x: %f, %f, %f \n", n[0].x, n[0].y, n[0].z);
    printf("Normal y: %f, %f, %f \n", n[1].x, n[1].y, n[1].z);
    printf("Normal z: %f, %f, %f \n", n[2].x, n[2].y, n[2].z);
    printf("Normal Length: %f, %f, %f \n", vlength(n[0]), vlength(n[1]), vlength(n[2]));

    printf("<x,y>: %f \n",  iproduct(n[0], n[1]));
    printf("<x,z>: %f \n",  iproduct(n[0], n[2]));
    printf("<y,z>: %f \n",  iproduct(n[1], n[2]));
}


struct VoxelList* createVoxelList(int xdim, int ydim, int zdim) {
    struct VoxelList* volist = (struct VoxelList*) malloc(sizeof(struct VoxelList));
    volist->vosize = 0.2;
    volist->list = (struct Voxel*) malloc(xdim*ydim*zdim*sizeof(struct Voxel));
    return volist;
}


int init7() {
    vo = createVoxelList();
    vo->xdim = 8;
    vo->ydim = 8;
    vo->zdim = 9;
}

int _draw_test2() {

}

///deprecate below line

struct Skeleton* s1;

int init5() {
    s1 = (struct Skeleton*) malloc(sizeof(struct Skeleton));
    return 0;
}


struct Skeleton* _create_skeleton(int nparts, float x, float y, float z) {
    struct Skeleton* skel = (struct Skeleton*) malloc(sizeof(struct Skeleton));
    //skel->nparts;
    return skel;
}

struct SkeletonPart* s2;

int init6() {
    s2= (struct SkeletonPart*) malloc(sizeof(struct SkeletonPart));
    s2->center.x = 0;
    s2->center.y = 0;
    s2->center.z = 3;
    s2->xsize = 1;
    s2->ysize = 1;
    s2->zsize = 1;
    s2->theta = 0; //increment these and see if it rotates
    s2->phi = 0;

    return 0;
}


int _draw_test() {
    //s2->theta += pi/150;
    s2->phi += pi/1024;
    if(s2->phi >= pi) {
        s2->phi -= pi;
    }
    draw_part(s2);
}

inline void compute_normals(struct SkeletonPart* skel) {
    //const struct Vector* center = &skel->center;
    float theta = skel->theta;
    float phi = skel->phi;
    struct Vector* n = skel->n;
    const float xsize = skel->xsize /2;
    const float ysize = skel->ysize /2;
    const float zsize = skel->zsize /2;

    float theta_90 = skel->theta+ pi/2;
    float phi_90 = skel->phi+ pi/2;

    n[0].x = xsize*cos(theta)*sin(phi+ pi/2);
    n[0].y = xsize*sin(theta)*sin(phi+ pi/2);
    n[0].z = xsize*cos(phi+ pi/2);

    n[1].x = ysize*cos(theta+ pi/2)*sin(phi+ pi/2);
    n[1].y = ysize*sin(theta+ pi/2)*sin(phi+ pi/2);
    n[1].z = ysize*cos(phi+ pi/2);

    n[2].x = zsize*cos(theta)*sin(phi);
    n[2].y = zsize*sin(theta)*sin(phi);
    n[2].z = zsize*cos(phi);

    printf("Normal x: %f, %f, %f \n", n[0].x, n[0].y, n[0].z);
    printf("Normal y: %f, %f, %f \n", n[1].x, n[1].y, n[1].z);
    printf("Normal z: %f, %f, %f \n", n[2].x, n[2].y, n[2].z);
    printf("Normal Length: %f, %f, %f \n", vlength(n[0]), vlength(n[1]), vlength(n[2]));

    printf("<x,y>: %f \n",  iproduct(n[0], n[1]));
    printf("<x,z>: %f \n",  iproduct(n[0], n[2]));
    printf("<y,z>: %f \n",  iproduct(n[1], n[2]));
}

/*
    v_set = [
        [0,0,0],
        [1,0,0],
        [1,1,0],
        [0,1,0],
        [0,0,1],
        [1,0,1],
        [1,1,1],
        [0,1,1]
    ]

    side_v = [
    [4,5,6,7],   #top (z=1)
    [0,1,2,3],   #bottom (z=0)
    [1,5,9,10],  #north (y=1)
    [7,3,11,8],  #south (y=0)
    [6,2,10,11], #west (x=0)
    [4,0,9,8],   #east (x=1)
    ]
*/
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

struct Vertex {
    float x,y,z;
};

int draw_part(struct SkeletonPart* skel) {
    compute_normals(skel);
    struct Vertex vlist[8];

    int i,j;
    for(i=0; i<8; i++) {
        vlist[i].x = 0;
        vlist[i].y = 0;
        vlist[i].z = 0;
        if(v_set[3*i+0] == 1) {
            vlist[i].x += skel->n[0].x;
            vlist[i].y += skel->n[0].y;
            vlist[i].z += skel->n[0].z;
        } else {
            vlist[i].x -= skel->n[0].x;
            vlist[i].y -= skel->n[0].y;
            vlist[i].z -= skel->n[0].z;
        }
        if(v_set[3*i+1] == 1) {
            vlist[i].x += skel->n[1].x;
            vlist[i].y += skel->n[1].y;
            vlist[i].z += skel->n[1].z;
        } else {
            vlist[i].x -= skel->n[1].x;
            vlist[i].y -= skel->n[1].y;
            vlist[i].z -= skel->n[1].z;
        }
        if(v_set[3*i+2] == 1) {
            vlist[i].x += skel->n[2].x;
            vlist[i].y += skel->n[2].y;
            vlist[i].z += skel->n[2].z;
        } else {
            vlist[i].x -= skel->n[2].x;
            vlist[i].y -= skel->n[2].y;
            vlist[i].z -= skel->n[2].z;
        }
        vlist[i].x += skel->center.x;
        vlist[i].y += skel->center.y;
        vlist[i].z += skel->center.z;

        printf("Vertex: %f, %f, %f \n", vlist[i].x, vlist[i].y, vlist[i].z);
    }
    struct Vertex* vt;
    //glEnable (GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    for(i=0; i<6;i++) {
/*
        q_set[4*i+0]
        q_set[4*i+1]
        q_set[4*i+2]
        q_set[4*i+3]
*/
/*
        if(i==0)
            glColor3ub(255,0,0);
        if(i==1)
            glColor3ub(255,255,0);
        if(i==2)
            glColor3ub(255,0,255);
        if(i==3)
            glColor3ub(0,0,0);
        if(i==4)
            glColor3ub(0,0,255);
        if(i==5)
            glColor3ub(0,255,0);
*/
        //glBegin(GL_QUADS);
            printf("Quad: \n");
            //for(j=0;j<=3;j++) {
            //    vt = &vlist[q_set[4*i+j]];
            //    glVertex3f(vt->x, vt->y, vt->z);

                if(i==0)
                    glColor3ub(255,0,0);
                if(i==1)
                    glColor3ub(255,255,0);
                if(i==2)
                    glColor3ub(255,0,255);
                if(i==3)
                    glColor3ub(255,255,255);
                if(i==4)
                    glColor3ub(0,0,255);
                if(i==5)
                    glColor3ub(0,255,0);

                //glTexCoord2i( 0, 0 );
                glVertex3f(vlist[q_set[4*i+0]].x,vlist[q_set[4*i+0]].y,vlist[q_set[4*i+0]].z);
                //glTexCoord2i( 1, 0 );
                glVertex3f(vlist[q_set[4*i+1]].x,vlist[q_set[4*i+1]].y,vlist[q_set[4*i+1]].z);
                //glTexCoord2i( 1, 1 );
                glVertex3f(vlist[q_set[4*i+2]].x,vlist[q_set[4*i+2]].y,vlist[q_set[4*i+2]].z);
                //glTexCoord2i( 0, 1 );
                glVertex3f(vlist[q_set[4*i+3]].x,vlist[q_set[4*i+3]].y,vlist[q_set[4*i+3]].z);

                vt = &vlist[q_set[4*i+j]];
                printf("%f, %f, %f \n",vt->x, vt->y, vt->z);


           // }


    }
    glEnd();
    glColor3ub(255,255,255);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glDisable (GL_DEPTH_TEST);
    //glDisable(GL_TEXTURE_2D);
    return 0;
}
