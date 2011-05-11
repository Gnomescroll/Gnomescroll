
import cython ##
import pyximport ##

from fast_map import *

class TerrainMap:

    def __init__(self):
        self.chunks = {}

    def get_chunk_list(self):
        for index in self.chunks.values()
            pass
            #index

    def set(int x,int y, int z,int value):
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            self.chunks[t] = MapChunk(t[0], t[1], t[2]) #new map chunk
        c = self.chunks[t]
        c.set(x,y,z)

    def get(int x,int y,int z):
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            return 0
        else:
            c = self.chunks[t]
            c.get(x,y,z)

class MapChunk:
    def __init__(self, int x_off, int y_off, int z_off):
        int_array_type = ctypes.c_int * 256 #map storage
        self.map_array = int_array_type()

        cdef int self.x_off = x_off*8
        cdef int self.y_off = y_off*8
        cdef int self.z_off = z_off*8

        self.index = (x_off, y_off, z_off)

    def set(self, int x, int y, int z, int value):
        self.map_array[x + 8*y + 8*8*z] = value

    def get(self, int x, int y, int z):
        return self.map_array[x + 8*y + 8*8*z]

    def serialize
