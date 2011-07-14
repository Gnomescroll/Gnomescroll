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

inline void compute_vo_normals(struct VoxelList* volist);
struct VoxelList* createVoxelList();
int init7();
int _draw_test2();

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

int _draw(struct VoxelList* vo);

//

int _draw(struct VoxelList* vo);
inline struct Voxel _get(struct VoxelList* vl, int x, int y, int z);
inline void _set(struct VoxelList* vl, int x, int y, int z, int r, int g, int b, int a);
struct VoxelList* _createVoxelList(int xdim, int ydim, int zdim, float x, float y, float z, float theta);
