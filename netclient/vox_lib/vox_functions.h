#include <compat_gl.h>

#include <stdio.h>

#include <math.h>
/*
Get drawing working first
Then do collision detection
*/

Vec3 {
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
    Vec3 center;
    Vec3 n[3]; //the normals; for internal cordinates
    Vec3 n2[3]; //normals for projection from cordinate system to internal
    float radius2; //used for hitscan pruning
};

__inline void compute_vo_normals(struct VoxelList* volist);
//struct VoxelList* createVoxelList();
//int init7();
//int _draw_test2();

int _draw(struct VoxelList* vo);
struct Voxel _get(struct VoxelList* vl, int x, int y, int z);
void _set(struct VoxelList* vl, int x, int y, int z, int r, int g, int b, int a);
void _set_voxel_color(struct Voxel* v, int r, int g, int b);
void _color(struct VoxelList* v1, int r, int g, int b, int base_r, int base_g, int base_b);
struct VoxelList* _createVoxelList(float vo_size, int xdim, int ydim, int zdim, float x, float y, float z, float theta);
//resize vorxel list?
int _deleteVoxelList(struct VoxelList* vo);
int _point_collision(struct VoxelList* vo, float x, float y, float z);
int _ray_cast_collision(struct VoxelList* vo, float x0, float y0, float z0, float x1, float y1, float z1);
int _raw_cast_collision(struct VoxelList* vo, float x, float y, float z, float x_angle, float y_angle);

int _raw_ray_cast_tracer(struct VoxelList* vo, float x, float y, float z, float x_angle, float y_angle);
int _ray_cast_tracer(struct VoxelList* vo, float x1, float y1, float z1, float x2, float y2, float z2);

