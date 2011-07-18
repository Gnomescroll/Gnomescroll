#from vox_dat import *

vox_id = 0
vox_dict = {}

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

    int _draw(VoxelList* vo)
    Voxel _get(VoxelList* vl, int x, int y, int z)
    void _set(VoxelList* vl, int x, int y, int z, int r, int g, int b, int a)
    VoxelList* _createVoxelList(float vo_size, int xdim, int ydim, int zdim, float x, float y, float z, float theta)
    int _deleteVoxelList(VoxelList* vo)
    int _point_collision(VoxelList* vo, float x, float y, float z)
    int _ray_cast_collision(VoxelList* vo, float x0, float y0, float z0, float x1, float y1, float z1)
    int _raw_cast_collision(VoxelList* vo, float x, float y, float z, float x_angle, float y_angle)

cdef class Vox:
    cdef VoxelList* vo
    cdef int id

    def __init__(self,x,y,z,theta, xdim, ydim, zdim, vosize=0.2):
        global vox_id
        self.vo = _createVoxelList(vosize, xdim, ydim, zdim, x, y, z, theta)
        self.id = vox_id #contains object it is associated with
        vox_id = 0

    def __del__(self):
        print "Vox deconstructor"
        _deleteVoxelList(self.vo)
        if self.id != 0:
            global vox_dict
            del vox_dict[self.id]

    def set_object(self, object): #use to set callback
        global vox_id, vox_dict
        self.id = vox_id #contains object it is associated with
        vox_id += 1
        vox_dict[self.id] = object

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
        d['voxels'] = self._dump_list()
        return d

    def collision_test(self, x,y,z):
        return _point_collision(self.vo,x,y,z)

    #v0 is point to cast from, v1 must be unit vector
    def ray_cast(self, x0,y0,z0, x1,y1,z1):
        return _ray_cast_collision(self.vo,x0,y0,z0, x1,y1,z1)

    def ray_cast2(self,x,y,z, x_angle, y_angle):
        _raw_cast_collision(self.vo,x,y,z, x_angle, y_angle)

import json

class Vox_loader:

    def __init__(self):
        pass

    def _get_path(self, file):
        if '/' in file:
            path = file
        else:
            path = './media/vox/' + file
        return path

    def load(self, file):
        path = self._get_path(file)
        try:
            with open(path) as f:
                j = json.load(f)
                dim = j['dim']
                vosize = j['vosize']
                voxels = j['voxels']
        except IOError:
            print "Error Loading: error in media/vox/%s failed" % (path,)
            return
        except KeyError:
            print 'Deserialized voxel data missing keys'
            return

        #try:
        x,y,z,theta = 0,0,0,0
        vox = Vox(x,y,z,theta, dim[0], dim[1], dim[2], vosize)
        for (x,y,z,r,g,b,a) in voxels:
            vox.set(x,y,z,r,g,b,a)
        return vox
        #except:
        #    print "Error Loading: Cannot Creating Model for: %s" % (path,)

    def save(self, Vox, file):
        path = self._get_path(file)
        with open(path, 'w') as f:
            s = Vox.serialize()
            json.dump(s, f)
