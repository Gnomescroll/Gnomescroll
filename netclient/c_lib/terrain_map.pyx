

cdef extern from "./t_map/t_properties.h":
    int max_cubes

cdef extern from "../c_lib/t_map/t_map.hpp":
    int _set(int x, int y, int z, int value)
    int _get(int x, int y, int z)
    int _clear()

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

cdef extern from "../c_lib/t_map/t_map.hpp":
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

cdef extern from "./t_map/t_map.hpp":
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

def get_packed_chunk(xoff, yoff, zoff):
    cdef vm_chunk *c
    c = _get_chunk(xoff, yoff, zoff)
    if c == NULL:
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
        #print "%i = %i" % (n, array[n])
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

#cdef extern from 't_vbo.h':
cdef extern from './t_map/t_vbo.h':
    struct Vertex:
        float x,y,z
        float tx,ty
        unsigned char r,g,b,a
        unsigned char tex[4]
    struct VBO:
        int v_num
        Vertex* vlist
        int VBO_id

'''
PART 2: Properties
'''

#cdef extern from "./t_properties.h":
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
        int infinite_texture


#cdef extern from "./t_properties.h":
cdef extern from "./t_map/t_properties.h":
    int _init_cube_properties(int id, int active, int occludes, int solid, int gravity, int transparent)
    cubeProperties* _get_cube_list()
    cubeProperties* _get_cube(int id)
    void set_infinite_texture(int id, int texture)
    void init_t_properties()

cdef int infinite_texture_counter = 1


## Setup ##
from dat_loader import c_dat

def init_cube_properties(id=None):
    global c_dat

    def apply(id):
        global infinite_texture_counter
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

        #init infinite textures
        if(int(c_dat.get(id,'infinite_texture_level')) not in [2,3]):
            cp.infinite_texture = 0;
        else:
            if(c_dat.get(id,'infinite_texture_level') == 2):
                if cp.infinite_texture == 0:
                    cp.infinite_texture = infinite_texture_counter
                    for _i in range(0,16):
                        set_infinite_texture(infinite_texture_counter+_i, c_dat.get(id,'infinite_texture_array')[_i])
                    infinite_texture_counter += 16
                else:
                    for _i in range(0,16):
                        set_infinite_texture(cp.infinite_texture+_i, c_dat.get(id,'infinite_texture_array')[_i])
            if(c_dat.get(id,'infinite_texture_level') == 3):
                if cp.infinite_texture == 0:
                    cp.infinite_texture = infinite_texture_counter
                    for _i in range(0,81):
                        set_infinite_texture(infinite_texture_counter+_i, c_dat.get(id,'infinite_texture_array')[_i])
                    infinite_texture_counter += 81
                else:
                    for _i in range(0,81):
                        set_infinite_texture(infinite_texture_counter+_i, c_dat.get(id,'infinite_texture_array')[_i])
        #'infinite_texture_level' : 0,
        #'infinite_texture' : None,
        #'infinite_texture_array' : [],

    if id is None:
        for id in c_dat.dat:
            apply(id)
    else:
        apply(id)

def isActive(unsigned int id):
    return _get_cube(id).active
def isOcclude(int id):
    return _get_cube(id).occludes
def isTransparent(int id):
    return _get_cube(id).transparent
def isSolid(int id):
    return _get_cube(id).solid

'''
Part 3: Quad Cache
'''


#cdef extern from 't_vbo.h':
cdef extern from './t_map/t_vbo.h':
    Vertex* _get_quad_cache()

cdef float * v_index
v_index = <float*> malloc(72*sizeof(float))

'''
l = [
         0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
         1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
         0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #north
         0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #south
         0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #west
         1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #east
    ]
'''
'''
l = [
         0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
         1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
         0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #north
         1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #south
         0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #west
         0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #east
    ]
'''

#north/south is +/- x
#west/east is +/- y
l = [
        1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , #top
        0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , #bottom
        1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #north
        0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #south
        1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , #west
        0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #east
]

'''
 0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
 1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
 0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #north
 1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #south
 0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #west
 0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #east
'''

for i in range(0,72):
    v_index[i]=l[i]

cdef extern from "./t_map/t_properties.h":
    void _set_cube_side_texture(int id, int size, int tex_id)

def init_cube_side_texture():
    global c_dat
    for id in range(max_cubes):
        for side in range(6):
            texture_id = c_dat.get(id, 'texture_id')[side]
            _set_cube_side_texture(id, side, texture_id)

def init_quad_cache():
    global v_index
    global c_dat
    cdef Vertex* quad_cache
    quad_cache = _get_quad_cache()
    cdef Vertex* v
    cdef int id,side,vnum,index
    for id in range(max_cubes):
        for side in range(6):
            for vert_num in range(4):
                index = id*6*4+4*side+vert_num
                index2 = 12*side+3*vert_num
                v = &quad_cache[index]
                v.x = v_index[index2 + 0]
                v.y = v_index[index2 + 1]
                v.z = v_index[index2 + 2]
                v.r = 255
                v.g = 255
                v.b = 255
                v.a = 255
                tx,ty = get_cube_texture(id, side, vert_num) #tile_id, side, vert_num
                if c_dat.get(id, 'active') > 1: # wtf is 'active' > 1 supposed to mean
                    tx,ty = get_cube_texture_alt(id, side, vert_num) #tile_id, side, vert_num
                v.tx = tx
                v.ty = ty
                tex0, tex1, tex2 = get_tex_texture(id, side, vert_num)
                v.tex[0] = tex0
                v.tex[1] = tex1
                v.tex[2] = tex2

def get_cube_texture(int tile_id, int side, int vert_num):
    global c_dat
    margin = (1./16.) *0.001#*0.004
    texture_id = c_dat.get(tile_id, 'texture_id')[side]

    cdef cubeProperties* cp
    cp = _get_cube(tile_id)
    if cp.infinite_texture > 0:  #zero textures for blocks with multiple textures, such as infinite texture blocks
        texture_id = 0

    texture_order = c_dat.get(tile_id, 'texture_order')[side][vert_num]
    x = texture_id % 16
    y = (texture_id - (texture_id % 16)) / 16
    tx = float(x) * 1./16.
    ty = float(y) * 1./16.

    if cp.infinite_texture == 0:
        if vert_num == 0:
            tx += 0 +margin
            ty += 0 +margin
        elif vert_num == 1:
            tx += 0 +margin
            ty += 1./16. -margin
        elif vert_num == 2:
            tx += 1./16. -margin
            ty += 1./16. -margin
        elif vert_num == 3:
            tx += 1./16. -margin
            ty += 0 + margin
        else:
            print "Error!!!! set_tex invalid input"
            assert False
        return (tx,ty)
    else:
        if vert_num == 0:
            tx += 0 +margin
            ty += 0 +margin
        elif vert_num == 1:
            tx += 0 +margin
            ty += 1./16. -margin
        elif vert_num == 2:
            tx += 1./16. -margin
            ty += 1./16. -margin
        elif vert_num == 3:
            tx += 1./16. -margin
            ty += 0 + margin
        else:
            print "Error!!!! set_tex invalid input"
            assert False
        return (tx,ty)

def get_tex_texture(int tile_id, int side, int vert_num):
    global c_dat
    texture_id = c_dat.get(tile_id, 'texture_id')[side]

    cdef cubeProperties* cp
    cp = _get_cube(tile_id)
    if cp.infinite_texture > 0:  #zero textures for blocks with multiple textures, such as infinite texture blocks
        texture_id = 0
    
    if(texture_id < 0):
        texture_id = 255

    texture_order = c_dat.get(tile_id, 'texture_order')[side][vert_num]
    tx = 0
    ty = 0

    if cp.infinite_texture == 0:
        if vert_num == 0:
            tx += 0
            ty += 0
        elif vert_num == 1:
            tx += 0
            ty += 255
        elif vert_num == 2:
            tx += 255
            ty += 255
        elif vert_num == 3:
            tx += 255
            ty += 0
        return (tx,ty,texture_id)
    else:
        if vert_num == 0:
            tx += 0
            ty += 0
        elif vert_num == 1:
            tx += 0
            ty += 255
        elif vert_num == 2:
            tx += 255
            ty += 255
        elif vert_num == 3:
            tx += 255
            ty += 0
        return (tx,ty,texture_id)

#for crop blocks
def get_cube_texture_alt(tile_id, side, vert_num):
    global c_dat
    margin = (1./16.) *0.001#*0.004
    texture_id = c_dat.get(tile_id, 'texture_id')[side]
    texture_order = c_dat.get(tile_id, 'texture_order')[side][vert_num]
    x = texture_id % 16
    y = (texture_id - (texture_id % 16)) / 16
    tx = float(x) * 1./16.
    ty = float(y) * 1./16.

    if vert_num == 1:
        tx += 0 +margin
        ty += 0 +margin
    elif vert_num == 2:
        tx += 0 +margin
        ty += 1./16. -margin
    elif vert_num == 3:
        tx += 1./16. -margin
        ty += 1./16. -margin
    elif vert_num == 0:
        tx += 1./16. -margin
        ty += 0 + margin
    return (tx,ty)

## functions ##
'''
PART 3: Drawing Functions
'''
#cdef extern from 't_vbo.h':
cdef extern from "./t_map/t_vbo.h":
    int _draw_terrain()
    int _update_chunks()
    int _draw_terrain()
    int _set_camera(float x, float y, float z, float vx, float vy, float vz, float ux, float uy, float uz, float ratio, float viewangle)
    int _set_fulstrum_culling(int value)
    int _set_view_distance(int vd)
    #int _create_vbo(VBO* q_VBO, Vertex* v_list, int v_num)
    #int _delete_vbo(VBO* q_VBO)
    #int _start_vbo_draw()
    #int _draw_quad_vbo(VBO* q_VBO)
    #int _end_vbo_draw()

cpdef update_chunks():
    _update_chunks()

cpdef draw_terrain():
    _draw_terrain()

def camera_callback(float x, float y, float z, float vx, float vy, float ux, float uy, float uz, float vz, float ratio, float viewangle):
    _set_camera(x, y, z, vx, vy, vz, ux,uy,uz, ratio, viewangle)

def set_view_distance(int vd):
    _set_view_distance(vd)

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
    tile = _get(x,y,z)
    return isSolid(tile)


'''
PART 5: Network Interface
'''

#cdef extern from 't_vbo.h':
cdef extern from "./t_map/t_vbo.h":
    int* _chunk_request()

def chunk_request():
    cdef int* req
    req = _chunk_request()
    if req == NULL:
        return None
    else:
        return [req[0],req[1], req[2]]


'''
cdef extern from "net_lib.h":
    int _get_net_out(char* buffer, int* length)
    int _net_in(char* buffer, int length)

net_out = None

def set_network_out(b_out):
    global net_out
    net_out = b_out

def net_tick():
    cdef int length
    cdef char msg[2048]
    cdef bytes py_string
    global net_out
    if net_out == None:
        print "Must set terrain_map for net_out for binary messages out!"
        assert False:
    while True:
        _get_net_out(&msg, &length)
        if length = 0:
            break
        py_string = msg[:length]
        net_out(msg,length)

def net_in(str):
    cdef char* buffer
    cdef int length
    length = len(str)
    buffer = str
    _net_in(str, length)
'''


'''
PART 5.5 : visualization
'''

cdef extern from './t_map/t_viz.h':
    int _draw_vbo_indicator(float x, float y, float z)
    void _toggle_t_viz_vbo_indicator_style()
    int _start_frame()
    int _event(int id)
    int _end_frame()
    int _draw_perf_graph(float x, float y, float z)

cdef extern from "./t_map/t_vbo.h":
    void _toggle_terrain_map_blend_mode()
    void _refresh_map_vbo()
    void _toggle_z_buffer()

def draw_vbo_indicator(float x, float y, float z):
    _draw_vbo_indicator(x, y, z)

def toggle_t_viz_vbo_indicator_style(**m):
    _toggle_t_viz_vbo_indicator_style()

def toggle_terrain_map_blend_mode(**m):
    _toggle_terrain_map_blend_mode()

def refresh_map_vbo(**m):
    _refresh_map_vbo()

def toggle_z_buffer(**m):
    _toggle_z_buffer()


class Profiler:
    def start_frame(self):
        _start_frame()

    def event(self, int id):
        _event(id)

    def end_frame(self):
        _end_frame()

    def draw_perf_graph(self, float x, float y, float z):
        _draw_perf_graph(x,y,z)

'''
PART 6: Init
'''

#cdef extern from 't_vbo.h':
cdef extern from "./t_map/t_vbo.h":
    int _init_draw_terrain()

#cdef extern from "./t_map.hpp":
cdef extern from "./t_map/t_map.hpp":
    int _init_t_map()
#cdef extern from "./t_map_draw.h":
#    int _init_t_map_draw()

def init(inited=[0]):
    if inited[0]:
        raise Exception, "Attempt to init terrain_map twice"
    else:
        inited[0] += 1
        
    print "Init Terrain Map"

    _init_draw_terrain()
    init_t_properties()
    _init_t_map();

# these get loaded in _cube_inits after cube_dat is received from server
#    init_cube_properties()
#    init_quad_cache()
#    set_hud_cube_selector()

    
    #_init_t_map_draw()

def set_hud_cube_selector():
    global c_dat
    import c_lib.c_lib_hud as cHUD

    def apply(id):
        hud_img = c_dat.get(id,'hud_img')
        hud_pos = c_dat.get(id,'hud_pos')
        cHUD.CubeSelector.load_cube_properties(hud_pos, id, hud_img)

    for id in c_dat.dat:
        apply(id)


'''
Epilogue: Cube dat update callbacks
'''
def _cube_inits(id=None):
    init_cube_properties(id)
    init_cube_side_texture()
    init_quad_cache()
    set_hud_cube_selector()

c_dat.on_first_load = init
c_dat.on_change = _cube_inits


'''
Steve
'''
def clear():
    _clear()
