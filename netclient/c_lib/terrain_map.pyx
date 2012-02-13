

cdef extern from "../c_lib/t_map/t_map.hpp" namespace "t_map":
    void init_t_map()
    void init_for_draw()

'''
def _init_map():
    init_t_map()

def _init_map_for_draw():
    init_for_draw()
'''

cdef extern from "../c_lib/t_map/t_map.hpp":
    int _get(int x, int y, int z)



## functions ##
'''
PART 3: Drawing Functions
'''

cdef extern from "./t_map/t_vbo_update.hpp" namespace "t_map":
    int update_chunks()

cdef extern from "./t_map/t_vbo.hpp" namespace "t_map":
    int draw_map()

cpdef _update_chunks():
    update_chunks()

cpdef _draw_map():
    draw_map()


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

def init_cube_properties():
    global c_dat

    def apply(id):
        cdef cubeProperties* cp
        cp = get_cube(id)
        #print "id= %s" % ( c_dat.get(id,'active') )

        #print self.dat
        #print type, prop

        cp.active = int(c_dat.get(id,'active'))
        cp.solid = int(c_dat.get(id,'solid'))
        cp.occludes = int(c_dat.get(id,'occludes'))
        cp.transparent = int(c_dat.get(id,'transparent'))
        cp.max_damage = int(c_dat.get(id,'max_damage'))

    for id in c_dat.dat:
        apply(id)


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

'''
    Set the textures on sides of cube
'''

cdef extern from "./t_map/texture.hpp" namespace "t_map":
    void set_cube_side_texture(int id, int side, int tex_id)

def init_cube_side_texture():
    global c_dat
    for id in c_dat.dat:
        for side in range(6):
            texture_id = c_dat.get(id, 'texture_id')[side]
            set_cube_side_texture(id, side, texture_id)

'''
init stuff
'''

def init():
    print "Init Terrain Map"
    init_cube_properties()
    init_cube_side_texture()
    init_t_map()
    set_hud_cube_selector()
    init_for_draw()


#DEPRECATE BELOW LINE

'''
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
'''