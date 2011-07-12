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
};

struct Voxel {
    unsigned char r,g,b,a;
};

struct VoxelList {
    int num;
    struct Voxel* list;
    int xdim, ydim, zdim;
    float vosize;
    float theta;
    struct Vector center;
    struct Vector n[3]; //the normals
};

int init7();
inline void compute_vo_normals(struct VoxelList* volist);
struct VoxelList* createVoxelList();

// deprecate below line

struct SkeletonPart {
    int nparts;
    struct SkeletonParts* parts;
    float theta, phi;
    struct Vector center;
    float xsize, ysize, zsize;
    struct Vector n[3]; //the normals x,y,z
};

struct Skeleton {
    int nparts;
    struct SkeletonParts* partListopen;

    float x, y, z;
    float theta;
    int nvertex;
    //struct Vertex vertexList;
};

int draw_part(struct SkeletonPart* skel);

int init6();
int _draw_test();
int _draw_test2();
