#include <SDL.h>
#include <SDL_opengl.h>

#include <stdio.h>

#include <math.h>
/*
Get drawing working first
Then do collision detection
*/

struct Vector {
    float x,y,z;
}

struct SkeletonPart {
    int nparts;
    struct SkeletonParts* parts;
    float theta, phi;
    Vector center;
    float xsize, ysize, zsize;
    Vector n[3] //the normals x,y,z
}

struct Skeleton {
    int nparts;
    struct SkeletonParts* partListopen;

    float x, y, z;
    float theta;
    int nvertex;
    //struct Vertex vertexList;
}

struct Skeleton* _create_skeleton(int nparts, float x, float y, float z) {
    struct* skel = (struct Skeleton*) malloc(sizeof(Skeleton));
    skel->nparts;

}


inline void compute_normals(struct SkeletonPart* skel) {
    //const struct Vector* center = &skel->center;
    const float theta = skel->theta;
    const float phi = skel->phi;
    const struct Vector* n = &skel->nx;
    const float xsize = skel->xsize /2;
    const float ysize = skel->ysize /2;
    const float zsize = skel->zsize /2;

    n[0] = xsize*cos(theta)*sin(phi);
    n[0] = ysize*sin(theta)*sin(phi);
    n[0] = zsize*cos(phi);

    ny.x = xsize*cos(theta)*sin(phi);
    ny.y = ysize*sin(theta)*sin(phi);
    ny.z = ysize*cos(phi);

    nz.x = zsize*cos(theta)*sin(phi);
    nz.y = zsize*sin(theta)*sin(phi);
    nz.z = zsize*cos(phi);

}

inline void compute_normal_y(struct SkeletonPart* skel) {

}

inline void compute_normal_z(struct SkeletonPart* skel) {

}

int draw_part(struct SkeletonPart* skel) {
}
