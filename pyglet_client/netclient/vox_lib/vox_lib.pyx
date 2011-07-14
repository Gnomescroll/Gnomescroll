#from vox_dat import *

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

cdef extern from 'vox_functions.h': #depcrecate
    int init7()
    int _draw_test2()

cdef extern from 'vox_functions.h':

    int _draw(VoxelList* vo)
    Voxel _get(VoxelList* vl, int x, int y, int z)
    void _set(VoxelList* vl, int x, int y, int z, int r, int g, int b, int a)
    VoxelList* _createVoxelList(float vo_size, int xdim, int ydim, int zdim, float x, float y, float z, float theta)
    int _deleteVoxelList(VoxelList* vo)

def test():
    print "Init"
    init7()

def draw():
    _draw_test2()
    #_draw_test()

cdef class Vox:
    cdef VoxelList* vo

    def __init__(self,xdim,ydim,zdim,x,y,z,theta):
        self.vo = _createVoxelList(xdim, ydim, zdim, x, y, z, theta)

    def __del__(self):
        _deleteVoxelList(self.vo)

    cpdef draw(self):
        _draw(self.vo)

    cpdef move(self, float x, float y,float z,float theta):
        self.vo.center.x = x
        self.vo.center.y = y
        self.vo.center.z = z
        self.vo.theta = theta

    cpdef set(self, int x,int y,int z,int r,int g, int b, int a):
        _set(self.vo, x,y,z,r,g,b,a)

    cpdef collision_check(self, float x,float y,float z):
        pass

    ## utility for editing ##
    def set_raw(self, int i, int r, int g,int b,int a):
        self.vo.list[i].r = r
        self.vo.list[i].g = g
        self.vo.list[i].b = b
        self.vo.list[i].b = a

    cdef Voxel get_raw(self.vo, o):
        return self.vo.i.list[i]

    cpdef collision_check(self, float x,float y,float z):
        pass


