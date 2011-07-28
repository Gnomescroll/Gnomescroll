import zlib
import struct

#cdef extern from "./clib/fast_map.c":
#    int hash_cord(int)


#define vm_map_dim 64 //number of map chunks in x/y
#define vm_chunk_size = 8
#define vm_column_max = 16

cdef enum:
    vm_map_dim = 64
    vm_chunk_size = 8
    vm_column_max = 16

cdef extern from "./t_map.h":

    struct vm_chunk:
        unsigned short voxel[512]
        int x_off, y_off, z_off
        unsigned int local_version
        unsigned int server_version

    struct vm_column:
        int x_off, y_off
        vm_chunk* chunk[vm_column_max]
        unsigned int local_version
        unsigned int server_version

    struct vm_map:
        vm_column column[vm_map_dim*vm_map_dim]

cdef extern from "./t_map.h":
    int init_t_map()
    int _set(int x, int y, int z, int value)
    int _get(int x, int y, int z)

    vm_map* _get_map()
    vm_chunk* _get_chunk(int xoff, int yoff, int zoff)
#done

cpdef inline set(int x,int y, int z,int value):
    _set(x,y,z,value)

cpdef inline int get(int x, int y,int z):
    _get(x,y,z)

#implement

def get_chunk_version_list():
    cdef vm_map* m
    cdef vm_chunk* c
    cdef int i,j
    m = _get_map()
    ll = []
    for i in range(0, vm_map_dim**2):
        m.column[i]
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != NULL:
                c = m.column[i].chunk[j]
                ll.append([c.x_off, c.y_off, c.z_off, c.local_version, c.server_version])
    return ll

cpdef get_chunk_list():
    cdef vm_map* m
    cdef vm_chunk* c
    cdef int i,j
    m = _get_map()
    ll = []
    print "get chunk list:"
    for i in range(0, vm_map_dim**2):
        m.column[i]
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != NULL:
                c = m.column[i].chunk[j]
                print "Not empty: %i, %i, %i" % (c.x_off, c.y_off, c.z_off)
                ll.append([c.x_off, c.y_off, c.z_off])
    print "end chunk list"
    return ll

cdef get_raw_chunk_list(): #DEPRECATE? USED by VBO.pyx
    assert False
    cdef vm_map *m
    cdef vm_chunk *c
    cdef int i,j
    m = _get_map()
    ll = []
    for i in range(0, vm_map_dim**2):
        m.column[i]
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != NULL:
                c = m.column[i].chunk[j]
                #ll.append(c)
    return ll

def get_packed_chunk(xoff, yoff, zoff):
    cdef vm_chunk *c
    c = _get_chunk(xoff, yoff, zoff)
    return zlib.compress(pack(c))

def set_packed_chunk(tmp):
    global fm_inv1, fm_inv2
    cdef int off_x, off_y, off_z, version, n, index
    cdef vm_chunk *c
    _set(8*off_x, 8*off_y, 8*off_z, 0)
    c = _get_chunk(off_x, off_y, off_z)
    tmp = zlib.decompress(tmp)
    (off_x,off_y,off_z, server_version, array) = fm_inv1.unpack(tmp)
    array = list(fm_inv2.unpack(array))
    assert len(array) == 512
    for n in range(0,512):
        index
        c.voxel[n] = array[n]
    c.local_version = server_version
    c.server_version = server_version
    #chunk.update_VBO = 1
    return (off_x, off_y, off_z)

'''
cpdef inline set_server_version(int x, int y, int z, int version):
    cdef MapChunk c
    c = get_or_create_chunk(x,y,z)
    c.server_version = version
'''

cpdef inline set_server_version(int x, int y, int z, int version):
    print "set_server_version used?"
    assert False
#should used compiled form

import struct
# < little-endian, standard size, no alignment
fm_inv1 = struct.Struct('< 4i 1024s')
fm_inv2 = struct.Struct('< 512H')
fm = struct.Struct('< 4i 512H')
cdef pack(vm_chunk *c):
    global fm
    cdef int chunk_dim, chunk_offset, off_x, off_y, off_z, version

    version = c.local_version
    off_x = c.x_off
    off_y = c.y_off
    off_z = c.z_off
    l = []
    for i in range(0,512):
        l.insert(i, c.voxel[i])

    print str((off_x,off_y,off_z, version))
    print str(l)
    return fm.pack(off_x,off_y,off_z, version, *l)
