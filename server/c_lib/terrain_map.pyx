'''
Part 1: State

'''

import zlib
import struct

#define vm_map_dim 64 //number of map chunks in x/y
#define vm_chunk_size = 8
#define vm_column_max = 16

cdef enum:
    vm_map_dim = 64
    vm_chunk_size = 8
    vm_column_max = 16

cdef enum:
    _xmax = vm_map_dim * vm_chunk_size
    _ymax = vm_map_dim * vm_chunk_size
    _zmax = vm_column_max * vm_chunk_size

xmax = _xmax
ymax = _ymax
zmax = _zmax

cdef extern from "./t_map/t_map.h":

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

cdef extern from "./t_map/t_map.h":
    int _init_t_map()
    int _clear()

    int _set(int x, int y, int z, int value)
    int _get(int x, int y, int z)

    int _apply_damage(int x, int y, int z, int value)

    int _set_server_version(int x,int y,int z, int server_version)
    vm_map* _get_map()
    vm_chunk* _get_chunk(int xoff, int yoff, int zoff)

    int _get_highest_open_block(int x, int y, int n)
    int _get_lowest_open_block(int x, int y, int n)


cdef extern from "./t_map/t_compress.h":
    int map_save_to_disk(char* fn)
cdef extern from "./t_map/t_decompress.h":
    int map_load_from_disk(char* fn)
    
#done

cpdef inline set(int x,int y, int z,int value):
    _set(x,y,z,value)

cpdef inline int get(int x, int y,int z):
    return _get(x,y,z)

#implement

def get_server_chunk_list():
    cdef vm_map* m
    cdef vm_chunk* c
    cdef int i,j
    m = _get_map()
    ll = []
    for i in range(0, vm_map_dim**2):
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != NULL:
                c = m.column[i].chunk[j]
                ll.append([c.x_off, c.y_off, c.z_off, c.local_version])
    return ll

def get_chunk_version_list():
    cdef vm_map* m
    cdef vm_chunk* c
    cdef int i,j
    m = _get_map()
    ll = []
    for i in range(0, vm_map_dim**2):
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != NULL:
                c = m.column[i].chunk[j]
                ll.append([c.x_off, c.y_off, c.z_off, c.local_version, c.server_version])
    return ll

cpdef get_chunk_list():
    assert False
    cdef vm_map* m
    cdef vm_chunk* c
    cdef int i,j
    m = _get_map()
    ll = []
    #print "get chunk list:"
    for i in range(0, vm_map_dim**2):
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != NULL:
                c = m.column[i].chunk[j]
                #print "Not empty: %i, %i, %i" % (c.x_off, c.y_off, c.z_off)
                ll.append([c.x_off, c.y_off, c.z_off])
    #print "end chunk list"
    return ll

cdef get_raw_chunk_list(): #DEPRECATE? USED by VBO.pyx
    assert False
    cdef vm_map *m
    cdef vm_chunk *c
    cdef int i,j
    m = _get_map()
    ll = []
    for i in range(0, vm_map_dim**2):
        for j in range(0, vm_column_max):
            if m.column[i].chunk[j] != NULL:
                c = m.column[i].chunk[j]
                #ll.append(c)
    return ll

def get_packed_chunk(xoff, yoff, zoff):
    cdef vm_chunk *c
    c = _get_chunk(xoff, yoff, zoff)
    if c == NULL:
        print "get_packed_chunk: chunk does not exist: %i, %i, %i" %(xoff, yoff, zoff)
        return ''
    return zlib.compress(pack(c))

def set_packed_chunk(tmp):
    global fm_inv1, fm_inv2
    cdef int x_off, y_off, z_off, version, n, index
    cdef vm_chunk *c
    tmp = zlib.decompress(tmp)
    (x_off,y_off,z_off, server_version, array) = fm_inv1.unpack(tmp)
    array = list(fm_inv2.unpack(array))
    assert len(array) == 512
    _set(8*x_off, 8*y_off, 8*z_off, 0)
    c = _get_chunk(x_off, y_off, z_off)
    for n in range(0,512):
        c.voxel[n] = array[n]
    c.local_version = server_version
    c.server_version = server_version
    c.x_off = x_off
    c.y_off = y_off
    c.z_off = z_off
    #chunk.update_VBO = 1
    return (c.x_off, c.y_off, c.z_off)

'''
cpdef inline set_server_version(int x, int y, int z, int version):
    cdef MapChunk c
    c = get_or_create_chunk(x,y,z)
    c.server_version = version
'''

cpdef inline set_server_version(int x, int y, int z, int version):
    _set(8*x,8*y,8*z,0)
    _set_server_version(x,y,z, version)
    #print "set_server_version used?"
    #assert False
#should used compiled form

import struct
# < little-endian, standard size, no alignment
fm_inv1 = struct.Struct('< 4i 1024s')
fm_inv2 = struct.Struct('< 512H')
fm = struct.Struct('< 4i 512H')
cdef pack(vm_chunk *c):
    global fm
    cdef int i
    l = []
    for i in range(0,512):
        l.insert(i, c.voxel[i])
    #print str((c.x_off,c.y_off,c.z_off, c.local_version))
    #print str(l)
    return fm.pack(c.x_off,c.y_off,c.z_off, c.local_version, *l)

cpdef inline int apply_damage(int x, int y, int z, int dmg):
    cdef int dead = _apply_damage(x,y,z, dmg)
    if dead:
        set(x, y, z, 0)
    return dead

'''
PART 2: Properties

'''
cdef extern from "./t_map/t_properties.h":
    struct cubeProperties:
        int active
        int occludes
        int solid
        int gravity
        int transparent
        int max_damage
        int neutron_tolerance
        int nuclear

cdef extern from "./t_map/t_properties.h":
    int _init_cube_properties(int id, int active, int occludes, int solid, int gravity, int transparent)
    cubeProperties* _get_cube_list()
    cubeProperties* _get_cube(int id)

## Setup ##

'''
from dat.cube_dat import cube_list
def init_cube_properties():
    cdef cubeProperties* cp
    global cube_list
    for d in cube_list.values():
        id = int(d['id'])
        if id >= 1024 or id < 0:
            print "Error: cube id invalid"
            return
        cp = _get_cube(id)
        cp.active = int(d.get('active',1))
        cp.occludes = int(d.get('occludes', 0))
        cp.solid = int(d.get('solid', 1))
        cp.gravity = int(d.get('gravity', 0))
        cp.transparent = int(d.get('transparent', 0))
        cp.max_damage = int(d.get('max_damage', 3))
'''

import dats.loader as dat_loader
c_dat = dat_loader.c_dat

def init_cube_properties(c_dat):

    def apply(id):
        cdef cubeProperties* cp
        cp = _get_cube(id)
        cp.active = int(c_dat.get(id,'active'))
        cp.occludes = int(c_dat.get(id,'occludes'))
        cp.solid = int(c_dat.get(id,'solid'))
        cp.gravity = int(c_dat.get(id,'gravity'))
        cp.transparent = int(c_dat.get(id,'transparent'))
        cp.max_damage = int(c_dat.get(id,'max_damage'))
        cp.neutron_tolerance = int(c_dat.get(id,'neutron_tolerance'))
        cp.nuclear = int(c_dat.get(id,'nuclear'))

    for id in c_dat.dat:
        apply(id)

c_dat.on_reload = init_cube_properties

cpdef inline int isActive(unsigned int id):
    return _get_cube(id).active
cpdef inline int isOcclude(int id):
    return _get_cube(id).occludes
cpdef inline int isTransparent(int id):
    return _get_cube(id).transparent
cpdef inline int isSolid(int id):
    return _get_cube(id).solid

'''
PART 4: Utility Functions
'''

cpdef inline int collisionDetection(int x, int y, int z):
    cdef int tile
    tile = _get(x,y,z)
    return isSolid(tile)

def init():
    global c_dat
    print "Init Terrain Map"
    _init_t_map()
    init_cube_properties(c_dat)
init()

def clear():
    _clear()

# n defines how many continuous open spaces must exist
def get_highest_open_block(int x, int y, int n=1):
    return _get_highest_open_block(x,y,n)

def get_lowest_open_block(int x, int y, int n=1):
    return _get_lowest_open_block(x,y,n)


'''
Part 5: Serialization
'''

import os, os.path, time
map_file_location = './content/maps/%s'

def save_to_disk(fn=''):

    if not fn:
        fn = 'map_%0.2f' % (time.time())
        fn = fn.replace('.', '_')
    fn = fn.replace('/', '_')
    fn = map_file_location % (fn,)

    map_save_to_disk(fn);

def load_from_disk(fn=''):
    if not fn:
        fn = 'mapsave1'
        
    fn = map_file_location % (fn,)
    if not os.path.exists(fn):
        print "Map %s does not exist. Abort map load." % (fn,)
        return 1
        
    map_load_from_disk(fn)

'''
Part 6: Generation
'''
