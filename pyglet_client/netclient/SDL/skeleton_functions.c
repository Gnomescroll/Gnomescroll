#include "./skeleton_functions.h"


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
    s2->center.z = 0;
    s2->xsize = 1;
    s2->ysize = 1;
    s2->zsize = 1;
    s2->theta = 0; //increment these and see if it rotates
    s2->phi = 0;

    return 0;
}

#define pi 3.14159

int _draw_test() {
    s2->theta += pi/150;
    //s2->phi += pi/300;
    draw_part(s2);
}

inline void compute_normals(struct SkeletonPart* skel) {
    //const struct Vector* center = &skel->center;
    const float theta = skel->theta;
    const float phi = skel->phi;
    struct Vector* n = skel->n;
    const float xsize = skel->xsize /2;
    const float ysize = skel->ysize /2;
    const float zsize = skel->zsize /2;

    n[0].x = xsize*cos(theta)*sin(phi+ pi/2);
    n[0].y = ysize*sin(theta)*sin(phi+ pi/2);
    n[0].z = zsize*cos(phi+ pi/2);

    n[1].x = xsize*cos(theta+ pi/2)*sin(phi+ pi/2);
    n[1].y = ysize*sin(theta+ pi/2)*sin(phi+ pi/2);
    n[1].z = ysize*cos(phi+ pi/2);

    n[2].x = zsize*cos(theta)*sin(phi);
    n[2].y = zsize*sin(theta)*sin(phi);
    n[2].z = zsize*cos(phi);

    printf("Normal x: %f, %f, %f \n", n[0].x, n[0].y, n[0].z);
    printf("Normal y: %f, %f, %f \n", n[1].x, n[1].y, n[1].z);
    printf("Normal z: %f, %f, %f \n", n[2].x, n[2].y, n[2].z);
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
        1,2,3,4,
        2,3,6,7,
        0,1,4,5,
        0,3,4,7,
        1,2,5,6 };

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
    glColor3ub(255,0,0);
    for(i=0; i<6;i++) {
/*
        q_set[4*i+0]
        q_set[4*i+1]
        q_set[4*i+2]
        q_set[4*i+3]
*/
        glBegin(GL_QUADS);
            printf("Quad: \n");
            for(j=0;j<=3;j++) {
                vt = &vlist[q_set[4*i+j]];
                glVertex3f(vt->x, vt->y, vt->z);
                printf("%f, %f, %f \n",vt->x, vt->y, vt->z);
            }
        glEnd();

    }
    return 0;
}
