


cdef extern from "./clib/fast_map.c":
    int hash_cord(int)

cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8

cdef class TerrainMap:

    chunks = {}
    l = []

    def __init__(self):
        pass
        #self.chunks = {}

    def get_chunk_list(self):
        l = []
        for index in self.chunks.keys():
            l.append(index)
        return l

    def get_chunk(self, index):
        pass

    def get_packed_chunk(self):
        for x in self.chunks.values():
            return pack(x)

    cpdef inline set(TerrainMap self, int x,int y, int z,int value):
        cdef MapChunk c
        cdef int _x, _y, _z
        #t = (hash_cord(x), hash_cord(y), hash_cord(z))
        _x = x - (x%x_chunk_size)
        _y = y - (y%y_chunk_size)
        _z = z - (z%z_chunk_size)
        t = (_x,_y,_z)

        if not self.chunks.has_key(t):
            self.chunks[t] = MapChunk(_x, _y, _z) #new map chunk
        c = self.chunks[t]
        c.set(x,y,z, value)

    cpdef inline int get(TerrainMap self, int x,int y,int z):
        cdef MapChunk c
        cdef int _x, _y, _z
        #t = (hash_cord(x), hash_cord(y), hash_cord(z))
        _x = x - (x%x_chunk_size)
        _y = y - (y%y_chunk_size)
        _z = z - (z%z_chunk_size)
        t = (_x,_y,_z)
        if not self.chunks.has_key(t):
            return 0
        c = self.chunks[t]
        return c.get(x,y,z)

cdef class MapChunk:
    cdef int index[3]
    cdef int map_array[512]
    cdef unsigned int local_version
    cdef unsigned int last_server_version

    def __init__(self, int x_off, int y_off, int z_off, int local_version = 0, int last_server_version = 0):
        self.local_version = local_version
        self.last_server_version = 0

        self.index[0] = x_off
        self.index[1] = y_off
        self.index[2] = z_off

        for i in range(0, 256):
            self.map_array[i] = 0

    cdef inline void set(self, int x, int y, int z, int value):
        self.version += 1
        x -= self.index[0]
        y -= self.index[1]
        z -= self.index[2]
        self.map_array[x + 8*y + 8*8*z] = value

    cdef inline int get(self, int x, int y, int z):
        x -= self.index[0]
        y -= self.index[1]
        z -= self.index[2]
        return self.map_array[x + 8*y + 8*8*z]

    cdef inline int set_last_server_version(self, int version):
        self.last_server_version = version

#should used compiled form

import struct

fm = struct.Struct('B H 3i 512H')
def pack(MapChunk mapChunk):
    global fm
    cdef int chunk_dim, chunk_offset, off_x, off_y, off_z
    chunk_dim = 8 #short
    chunk_offset = 0 #offset into chunk
    off_x = mapChunk.index[0]
    off_y = mapChunk.index[1]
    off_z = mapChunk.index[2]
    l = []
    for i in range(0,512):
        l.insert(i, mapChunk.map_array[i])
    return fm.pack(chunk_dim, chunk_offset, off_x,off_y,off_z, *l)
