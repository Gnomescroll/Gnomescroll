
import cython ##
import pyximport ##

from fast_map import *

cdef inline set(map_array, int x, int y, int z, int value):
        map_array[x + 8*y + 8*8*z] = value

cdef inline int get(map_array, int x, int y, int z):
        map_array[x + 8*y + 8*8*z]


##test
cdef struct t_struct:
    int value1
    int value2

#cdef extern t_struct* getthem()

##

class TerrainMap:

    def __init__(self):
        self.chunks = {}

    def get_chunk_list(self):
        l = []
        for index in self.chunks.keys():
            l.append(index)
        return l

    def get_chunk(self, index):
        pass

    def set(int x,int y, int z,int value):
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            self.chunks[t] = MapChunk(t[0], t[1], t[2]) #new map chunk
        c = self.chunks[t]
        #c.set(x,y,z)
        set(c.map_array, x, y, z, value)

    def get(int x,int y,int z):
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            return 0

        c = self.chunks[t]
        #c.get(x,y,z)
        get(c.map_array,x,y,z)

cdef class MapChunk:
    cdef int index[3]
    cdef int map_array[256]

    def __init__(self, int x_off, int y_off, int z_off):
        #int_array_type = ctypes.c_int * 256 #map storage
        #self.map_array = int_array_type()

        self.index[0] = x_off
        self.index[1] = y_off
        self.index[2] = z_off

        for i in range(0, 256):
            self.map_array[i] = 0

#        self.x_off = x_off*8
#        self.y_off = y_off*8
#        self.z_off = z_off*8
#        self.index = (x_off, y_off, z_off)

#    def set(self, int x, int y, int z, int value):
#        self.map_array[x + 8*y + 8*8*z] = value

#    def get(self, int x, int y, int z):
#        return self.map_array[x + 8*y + 8*8*z]

#    def serialize(self):
#        return (self.index, self.map_array)
