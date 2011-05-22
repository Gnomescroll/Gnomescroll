
#cdef struct t_struct:
#    int value1
#    int value2


cdef extern from "./clib/fast_map.c":
    int hash_cord(int)

import zlib
#import array

cdef class TerrainMap:

    chunks = {}
    l = []

    def get_chunk_list(self):
        l = []
        #for index in self.chunks.keys():
        #    l.append(index)
        cdef MapChunk c
        for c in self.chunks.values():
            l.append([c.index[0], c.index[1], c.index[1], c.version])
        return l

    def get_chunk(self, int x, int y, int z):
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            return 0
        return self.chunks[t]

    def get_packed_chunk(self, x, y, z):
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            return ''
        t = self.chunks[t]
        return zlib.compress(pack(t))

    def set_packed_chunk(self, tmp):
        tmp = zlib.decompress(tmp)
        global fm_inv1, fm_inv2
        (off_x,off_y,off_z, version, array) = fm_inv1.unpack(tmp)
        array = list(fm_inv2.unpack(array))
        print "unpacking"
        print str((off_x,off_y,off_z, version))
        print str(array)

    cpdef inline set(self, int x,int y, int z,int value):
        cdef MapChunk c
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            self.chunks[t] = MapChunk(8*t[0], 8*t[1], 8*t[2]) #new map chunk
        c = self.chunks[t]
        c.set(x,y,z, value)

    cpdef inline int get(TerrainMap self, int x,int y,int z):
        cdef MapChunk c
        t = (hash_cord(x), hash_cord(y), hash_cord(z))
        if not self.chunks.has_key(t):
            return 0
        c = self.chunks[t]
        return c.get(x,y,z)

cdef class MapChunk:
    cdef int index[3]
    cdef int map_array[512]
    cdef unsigned int version

    def __init__(self, int x_off, int y_off, int z_off):
        self.version = 0
        self.index[0] = x_off
        self.index[1] = y_off
        self.index[2] = z_off

        for i in range(0, 512):
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

def mapChunkSignature(mapChunk):
    return (mapChunk.index[0], mapChunk.index[1], mapChunk.index[2], mapChunk.version)

#should used compiled form

import struct

fm_inv1 = struct.Struct('< 4i 1024s')
fm_inv2 = struct.Struct('< 512H')

fm = struct.Struct('< 4i 512H')
def pack(MapChunk mapChunk):
    global fm
    cdef int chunk_dim, chunk_offset, off_x, off_y, off_z, version

    version = mapChunk.version
    off_x = mapChunk.index[0]
    off_y = mapChunk.index[1]
    off_z = mapChunk.index[2]
    l = []
    for i in range(0,512):
        l.insert(i, mapChunk.map_array[i])
    #print str((off_x,off_y,off_z, version))
    #print str(l)
    return fm.pack(off_x,off_y,off_z, version, *l)
