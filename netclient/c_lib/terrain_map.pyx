

cdef extern from "../c_lib/t_map/t_map.hpp" namespace "t_map":
    void init_t_map()
    void init_for_draw()

def _init_map():
    init_t_map()

def _init_map_for_draw():
    init_for_draw()

cdef extern from "../c_lib/t_map/t_map.hpp" namespace "t_map":
    int get(int x, int y, int z)

'''
PART 2: Properties
'''

cdef extern from "./t_map/t_properties.hpp" namespace "t_map":
    struct cubeProperties:
        bint active
        bint solid
        bint occludes
        bint transparent
        bint reserved5
        bint reserved6
        bint reserved7
        bint reserved8
        unsigned char max_damage

cdef extern from "./t_map/t_properties.hpp" namespace "t_map":
    cubeProperties* get_cube(int id)

## Setup ##
from dat_loader import c_dat

def init_cube_properties(id=None):
    global c_dat

    def apply(id):
        global infinite_texture_counter
        cdef cubeProperties* cp
        cp = get_cube(id)
        cp.active = int(c_dat.get(id,'active'))
        cp.solid = int(c_dat.get(id,'solid'))
        cp.occludes = int(c_dat.get(id,'occludes'))
        cp.transparent = int(c_dat.get(id,'transparent'))
        cp.max_damage = int(c_dat.get(id,'max_damage'))

    if id is None:
        for id in c_dat.dat:
            apply(id)
    else:
        apply(id)

'''
    Set the textures on sides of cube
'''

cdef extern from "./t_map/texture.hpp" namespace "t_map":
    void set_cube_side_texture(int id, int side, int tex_id)

def init_cube_side_texture():
    global c_dat
    for id, cube in c_dat:
        for side in range(6):
            texture_id = cube['texture_id'][side]
            set_cube_side_texture(id, side, texture_id)

'''
Part 3: Quad Cache
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

'''
GLSL_TEXTURE_ARRAY = True

def init_quad_cache():
    global v_index
    global c_dat
    global GLSL_TEXTURE_ARRAY
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

                if GLSL_TEXTURE_ARRAY:
                    tx,ty,tz = get_tex_texture(id, side, vert_num) #tile_id, side, vert_num
                    if c_dat.get(id, 'active') > 1: # wtf is 'active' > 1 supposed to mean
                        tx,ty,tz = get_cube_texture_alt(id, side, vert_num) #tile_id, side, vert_num
                else:
                    tx,ty,tz = get_cube_texture(id, side, vert_num) #tile_id, side, vert_num
                    if c_dat.get(id, 'active') > 1: # wtf is 'active' > 1 supposed to mean
                        tx,ty,tz = get_cube_texture_alt(id, side, vert_num) #tile_id, side, vert_num
                    tz = 0.0
                v.tx = tx
                v.ty = ty
                v.tz = tz

def get_cube_texture(int tile_id, int side, int vert_num):
    global c_dat
    margin = (1./16.) *0.001#*0.004
    texture_id = c_dat.get(tile_id, 'texture_id')[side]
    if texture_id < 0:
        texture_id = 255

    cdef cubeProperties* cp
    cp = _get_cube(tile_id)

    texture_order = c_dat.get(tile_id, 'texture_order')[side][vert_num]
    x = texture_id % 16
    y = (texture_id - (texture_id % 16)) / 16
    tx = float(x) * 1./16.
    ty = float(y) * 1./16.

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
    return (tx,ty,texture_id)

def get_tex_texture(int tile_id, int side, int vert_num):
    global c_dat
    texture_id = c_dat.get(tile_id, 'texture_id')[side]

    cdef cubeProperties* cp
    cp = _get_cube(tile_id)

    texture_order = c_dat.get(tile_id, 'texture_order')[side][vert_num]
    tx = 0
    ty = 0

    if vert_num == 0:
        tx = 0.0
        ty = 0.0
    elif vert_num == 1:
        tx = 0.0
        ty = 1.0
    elif vert_num == 2:
        tx = 1.0
        ty = 1.0
    elif vert_num == 3:
        tx = 1.0
        ty = 0.0
    return (tx,ty,texture_id)
'''

## functions ##
'''
PART 3: Drawing Functions
'''


cdef extern from "./t_map/t_vbo_update.hpp" namespace "t_map":
    int update_chunks()

cdef extern from "./t_map/t_vbo_draw.hpp" namespace "t_map":
    int draw_map()

cpdef _update_chunks():
    update_chunks()

cpdef _draw_terrain():
    draw_map()


'''
Hud Selector Stuff
'''
cdef extern from "./hud/cube_selector.hpp" namespace "HudCubeSelector":
    cdef cppclass CubeSelector:
        void load_cube_property(int pos, int cube_id, int tex_id)

    CubeSelector cube_selector

def set_hud_cube_selector():
    global c_dat

    for id in c_dat.dat:
        hud_img = c_dat.get(id,'hud_img')
        hud_pos = c_dat.get(id,'hud_pos')
        cube_selector.load_cube_property(hud_pos, id, hud_img)