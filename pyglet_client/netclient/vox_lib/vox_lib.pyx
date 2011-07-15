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
    int _point_collision(VoxelList* vo, float x, float y, float z)

def test():
    print "Init"
    init7()

def draw():
    _draw_test2()
    #_draw_test()

cdef class Vox:
    cdef VoxelList* vo

    def __init__(self,x,y,z,theta, xdim, ydim, zdim):
        vosize = 0.2
        self.vo = _createVoxelList(vosize, xdim, ydim, zdim, x, y, z, theta)

    def __del__(self):
        print "Vox deconstructor"
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

    ## utility for editing ##
    def set_raw(self, int i, int r, int g,int b,int a):
        self.vo.list[i].r = r
        self.vo.list[i].g = g
        self.vo.list[i].b = b
        self.vo.list[i].b = a

    cdef Voxel get_raw(self, int i):
        return self.vo.list[i]

    cpdef collision_check(self, float x,float y,float z):
        pass

    def dump_list(self): # serialize to (x,y,x,r,g,b,a) pairs
        cdef Voxel v
        l = []
        for x in range(0,self.vo.xdim):
            for y in range(0, self.vo.ydim):
                for z in range(0, self.vo.zdim):
                    v = _get(self.vo, x,y,z)
                    l.append([x,y,z, v.r, v.g, v.b, v.a])

    def serialize(self):
        d= {}
        d['vosize'] = self.vo.vosize
        d['dim'] = [self.vo.xdim, self.vo.ydim, self.vo.zdim]
        d['list'] = self._dump_list()
        return d

    def collisin_test(self, x,y,z):
        i = _point_collision(self.vo,x,y,z)
        print str(i)


import json

class Vox_loader:

    def __init__(self):
        pass

    def load(self, file):
        try:
            FILE = open(".media/vox/"+file,"r")
            l = json.load(FILE)
            xdim, ydim, zdim = l['dim']
            vosize = l['vosize']
            list = l['list']
        except:
            print "Error Loading: error in media/vox/%s failed" % (file)
        try:
            x,y,z,theta = 0,0,0,0
            vox = Vox(vosize, xdim, ydim, zdim, x,y,z,theta)
            for (x,y,z,r,g,b,a) in list:
                vox.set(x,y,z,r,g,b,a)
            return vox
        except:
            print "Error Loading: Cannot Creating Model for:" % (file)

    def save(self, Vox, file):
        FILE = open(".media/vox/"+file,"w")
        s = Vox.serialize()
        json.dump(s, file)

