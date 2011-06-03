import zlib
import struct

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

    def get_chunk_version_list(self):
        l = []
        cdef MapChunk c
        for c in self.chunks.values():
            l.append([c.index[0], c.index[1], c.index[2], c.version, c.server_version])
        return l


#    def get_chunk(TerrainMap self, int x, int y, int z):
#        t = (x >> 3, y >> 3, z >> 3)
#        return self.chunks.get(t, 0)

    def get_or_create_chunk(TerrainMap self, int x, int y, int z):
        t = (x >> 3, y >> 3, z >> 3)
        cdef MapChunk mc
        if not self.chunks.has_key(t):
            self.chunks[t] = MapChunk(x, y, z) #new map chunk
        return self.chunks[t]

    def get_packed_chunk(self, x, y, z):
        t = (x >> 3, y >> 3, z >> 3)
        if not self.chunks.has_key(t):
            return ''
        t = self.chunks[t]
        return zlib.compress(pack(t))

    def set_packed_chunk(self, tmp):
        global fm_inv1, fm_inv2
        cdef int off_x, off_y, off_z, version
        cdef MapChunk chunk
        tmp = zlib.decompress(tmp)
        (off_x,off_y,off_z, version, array) = fm_inv1.unpack(tmp)
        array = list(fm_inv2.unpack(array))
        chunk = self.get_or_create_chunk(off_x, off_y, off_z)
        chunk.version = version
        assert len(array) == 512
        for n in range(0,512):
            chunk.map_array[n] = array[n]
        return (off_x, off_y, off_z)

#    cpdef inline int get_version(self, x,y,z):
#        cdef MapChunk c
#        t = (x >> 3, y >> 3, z >> 3)
#        if not self.chunks.has_key(t):
#            return -1 #chunk does not exist
#        else:
#            c = self.chunks[t]
#            return c.version

    cpdef inline set_server_version(TerrainMap self, int x, int y, int z, int version):
        cdef MapChunk c
        c = self.get_or_create_chunk(x,y,z)
        c.server_version = version

    cpdef inline set(TerrainMap self, int x,int y, int z,int value):
        cdef MapChunk c
        t = (x >> 3, y >> 3, z >> 3)
        if not self.chunks.has_key(t):
            self.chunks[t] = MapChunk(x,y,z) #new map chunk
        c = self.chunks[t]
        c.set(x,y,z, value)

    cpdef inline int get(TerrainMap self, int x, int y,int z):
        cdef MapChunk c
        t = (x >> 3, y >> 3, z >> 3)
        if not self.chunks.has_key(t):
            return 0
        c = self.chunks[t]
        return c.get(x,y,z)

cdef class MapChunk:
    #cdef int index[3]
    #cdef int map_array[512]
    #cdef unsigned int version
    #cdef unsigned int server_version

    def __init__(self, int x_off, int y_off, int z_off, int version = 0, int server_version = 0):
        self.version = version
        self.server_version = 0

        self.index[0] = x_off - (x_off % 8)
        self.index[1] = y_off - (y_off % 8)
        self.index[2] = z_off - (z_off % 8)

        for i in range(0, 512):
            self.map_array[i] = 0

    cpdef inline set(MapChunk self, int x, int y, int z, int value):
        self.version += 1
        x -= self.index[0]
        y -= self.index[1]
        z -= self.index[2]
        self.map_array[x + 8*y + 8*8*z] = value

    cpdef inline int get(MapChunk self, int x, int y, int z):
        x -= self.index[0]
        y -= self.index[1]
        z -= self.index[2]
        return self.map_array[x + 8*y + 8*8*z]

    cdef inline int set_server_version(MapChunk self, int version):
        self.server_version = version

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

    print str((off_x,off_y,off_z, version))
    print str(l)
    return fm.pack(off_x,off_y,off_z, version, *l)
