#from vox_dat import *

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


cdef extern from 'vox_functions.h':
    cdef struct Vector:
        float x,y,z
    cdef struct Voxel:
        unsigned char r,g,b,a
    cdef struct VoxelList:
        int num
        Voxel* list
        int xdim, ydim, zdim
        float vosize
        float theta
        Vector center
        Vector n[3]

cdef extern from 'vox_functions.h':
    int init7()
    int _draw_test2()

    int _draw(struct VoxelList* vo)
    Voxel _get(struct VoxelList* vl, int x, int y, int z)
    void _set(struct VoxelList* vl, int x, int y, int z, int r, int g, int b, int a)
    VoxelList* _createVoxelList(int xdim, int ydim, int zdim, float x, float y, float z, float theta)

def test():
    print "Init"
    init7()

def draw():
    _draw_test2()
    #_draw_test()

cdef class Vox:

    def __init__(self):
        pass

    def __del__(self):
        pass

    cpdef collision_check(self, float x,float y,float z):
        pass


