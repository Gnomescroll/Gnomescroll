import zlib
import struct

#cdef extern from "./clib/fast_map.c":
#    int hash_cord(int)


#define vm_map_dim 64 //number of map chunks in x/y
#define vm_chunk_size = 8
#define vm_column_max = 16

cdef enum:
    vm_map_dim 64
    vm_chunk_size 8
    vm_column_max 16

cdef extern from "./t_map.h":
    struct

    struct vm_chunk
        ushort voxel[512]
        int x_off, y_off, z_off
        uint local_version
        uint server_version

    struct vm_column
        int x_off, y_off
        vm_chunk* chunk[vm_column_max]
        uint local_version
        uint server_version

    struct vm_map
        vm_column* column[ma_dim*ma_dim]

cdef extern from "./t_map.h":
    int init_t_map()
    int _set(int x, int y, int z, int value)
    int _get(int x, int y, int z)
    vm_map* _get_map()

#done

cpdef inline set(int x,int y, int z,int value):
    _set(x,y,z,value)

cpdef inline int get(int x, int y,int z):
    _get(x,y,z)

#implement

def get_chunk_version_list():
    cdef vm_map m
    cdef vm_chunk c
    cdef int i,j
    m = _get_map()
    ll = []
    for i in range(0, vm_map_dim**2):
        m.column[i]
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != 0:
                c = m.column[i].chunk[j]
                ll.append([c.x_off, c.y_off, c.z_off, c.local_version, c.server_version])
    return ll

cpdef get_chunk_list():
    cdef vm_map m
    cdef vm_chunk c
    cdef int i,j
    m = _get_map()
    ll = []
    print "get chunk list:"
    for i in range(0, vm_map_dim**2):
        m.column[i]
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != 0:
                c = m.column[i].chunk[j]
                print "Not empty: %i, %i, %i" % (c.x_off, c.y_off, c.z_off)
                ll.append([c.x_off, c.y_off, c.z_off])
    print "end chunk list"
    return ll

cdef get_raw_chunk_list(): #DEPRECATE?
    cdef vm_map m
    cdef vm_chunk c
    cdef int i,j
    m = _get_map()
    ll = []
    for i in range(0, vm_map_dim**2):
        m.column[i]
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != 0:
                c = m.column[i].chunk[j]
                ll.append(c)
    return c

def get_packed_chunk(x, y, z):
    global chunks
    t = (x >> 3, y >> 3, z >> 3)
    if not chunks.has_key(t):
        return ''
    t = chunks[t]
    return zlib.compress(pack(t))

def set_packed_chunk(tmp):
    global chunks
    global fm_inv1, fm_inv2
    cdef int off_x, off_y, off_z, version, n
    cdef MapChunk chunk
    tmp = zlib.decompress(tmp)
    (off_x,off_y,off_z, version, array) = fm_inv1.unpack(tmp)
    array = list(fm_inv2.unpack(array))
    #chunk = get_or_create_chunk(off_x, off_y, off_z)
    assert len(array) == 512
    for n in range(0,512):
        chunk.map_array[n] = array[n]
    chunk.version = version
    chunk.update_VBO = 1
    return (off_x, off_y, off_z)

'''
cpdef inline set_server_version(int x, int y, int z, int version):
    cdef MapChunk c
    c = get_or_create_chunk(x,y,z)
    c.server_version = version
'''

cpdef inline set_server_version(int x, int y, int z, int version):
    print "set_server_version used?"
#should used compiled form

import struct
# < little-endian, standard size, no alignment
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
