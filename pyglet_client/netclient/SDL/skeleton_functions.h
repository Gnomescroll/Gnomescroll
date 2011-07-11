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

int init6() ;
