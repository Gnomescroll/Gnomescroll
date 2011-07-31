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
    int _set(int x, int y, int z, int value)
    int _get(int x, int y, int z)

    int _set_server_version(int x,int y,int z, int server_version)
    vm_map* _get_map()
    vm_chunk* _get_chunk(int xoff, int yoff, int zoff)
#done

cpdef inline set(int x,int y, int z,int value):
    _set(x,y,z,value)

cpdef inline int get(int x, int y,int z):
    return _get(x,y,z)

#implement

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
    cdef vm_map* m
    cdef vm_chunk* c
    cdef int i,j
    m = _get_map()
    ll = []
    print "get chunk list:"
    for i in range(0, vm_map_dim**2):
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
    print str((c.x_off,c.y_off,c.z_off, c.local_version))
    #print str(l)
    return fm.pack(c.x_off,c.y_off,c.z_off, c.local_version, *l)


''' imports '''
from libc.stdlib cimport malloc, free

cdef extern from 'types.h':
    struct Vertex:
        float x,y,z
        float tx,ty
        unsigned char r,g,b,a
    struct VBO:
        int v_num
        Vertex* vlist
        int VBO_id

'''
PART 2: Properties

'''
cdef extern from "./t_properties.h":
    struct cubeProperties:
        int active
        int occludes
        int solid
        int gravity
        int transparent

cdef extern from "./t_properties.h":
    int _init_cube_properties(int id, int active, int occludes, int solid, int gravity, int transparent)
    cubeProperties* _get_cube_list()
    cubeProperties* _get_cube(int id)

## Setup ##
from cube_dat import cube_list


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

cpdef inline int isActive(unsigned int id):
    return _get_cube(id).active
cpdef inline int isOcclude(int id):
    return _get_cube(id).occludes
cpdef inline int isTransparent(int id):
    return _get_cube(id).transparent
cpdef inline int isSolid(int id):
    return _get_cube(id).solid

'''
Part 3: Quad Cache
'''

cdef extern from 't_properties.h':
    Vertex* _get_quad_cache()

cdef float * v_index
v_index = <float*> malloc(72*sizeof(float))

l = [
         0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
         1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
         0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #north
         0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #south
         0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #west
         1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #east
    ]

cdef enum:
    max_cubes = 1024

def init_quad_cache():
    global v_index
    cdef Vertex* quad_cache
    quad_cache = _get_quad_cache()
    cdef Vertex* v
    cdef int id,side,vnum,index
    for id in range(0, max_cubes):
        for side in range(0,6):
            for vert_num in range(0,4):
                index = id*6*4+4*side+vert_num
                index2 = 12*side+4*vert_num
                v = &quad_cache[index]
                v.x = v_index[index2 + 0]
                v.y = v_index[index2 + 1]
                v.z = v_index[index2 + 2]
                v.r = 255
                v.g = 255
                v.b = 255
                v.a = 255
                tx,ty = get_cube_texture(id, side, vert_num) #tile_id, side, vert_num
                v.tx = tx
                v.ty = ty

def get_cube_texture(tile_id, side, vert_num):
    global cube_list
    d = cube_list.get(tile_id, {})
    texture_id = d.get('texture_id', [0,1,2,3,4,5])[side]
    texture_order = d.get('texture_order', [[0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],])[side][vert_num]
    x = texture_id % 16
    y = (texture_id - (texture_id % 16)) / 16
    tx = float(x) * 1./16.
    ty = float(y) * 1./16.
    if vert_num == 0:
        tx += 0
        ty += 0
    elif vert_num == 1:
        tx += 1./16.
        ty += 0
    elif vert_num == 2:
        tx += 1./16.
        ty += 1./16.
    elif vert_num == 3:
        tx += 0
        ty += 1./16.
    else:
        print "Error!!!! set_tex invalid input"
        assert False
    return (tx,ty)

## functions ##
'''
PART 3: Drawing Functions

'''
cdef extern from 't_vbo.h':
    int _draw_terrain()
    int _update_chunks()
    int _draw_terrain()
    #int _create_vbo(VBO* q_VBO, Vertex* v_list, int v_num)
    #int _delete_vbo(VBO* q_VBO)
    #int _start_vbo_draw()
    #int _draw_quad_vbo(VBO* q_VBO)
    #int _end_vbo_draw()

cpdef update_chunks():
    _update_chunks()

cpdef draw_terrain():
    _draw_terrain()

### CLEANUP
'''
l = [0,0,1, 0,0,-1, 0,1,0, 0,-1,0, -1,0,0, 1,0,0]
cdef int s_array[3*6]
for i in range(0, 3*6):
    s_array[i] = l[i]

cdef inline _is_occluded(int x,int y,int z, int side_num):
    global s_array
    cdef int _x, _y, _z, tile_id,i

    i = s_array[3*side_num]
    _x = s_array[i+0] + x
    _y = s_array[i+1] + y
    _z = s_array[i+2] + z

    tile_id = terrain_map.get(_x,_y,_z)
    return isOcclude(tile_id)
'''
## CLEANUP

'''
PART 4: Utility Functions
'''
cpdef inline int collisionDetection(int x, int y, int z):
    cdef int tile
    tile = get(x,y,z)
    return isSolid(tile)

'''
PART 5: Init
'''

cdef extern from 't_vbo.h':
    int _init_draw_terrain()
cdef extern from "./t_map.h":
    int _init_t_map()
#cdef extern from "./t_map_draw.h":
#    int _init_t_map_draw()

def init():
    print "Init Terrain Map"
    init_cube_properties()
    init_quad_cache()
    _init_t_map();
    #_init_t_map_draw()
    _init_draw_terrain()
    assert False

init()
