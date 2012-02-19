from libcpp cimport bool

cdef extern from "./options.hpp" namespace "Options":
    void set_name(char* name)
    void set_server(char* server)
    void set_port(int port)
    void set_auto_assign_team(bool auto_assign_team)
    void set_width(int width)
    void set_height(int height)
    void set_fullscreen(bool fullscreen)
    void set_fov(float fov)
    void set_sensitivity(float sensitivity)
    void set_camera_speed(float camera_speed)
    void set_invert_mouse(bool invert_mouse)
    void set_hud(bool hud)
    void set_diagnostic_hud(bool diagnostic_hud)
    void set_fps(bool fps)
    void set_ping(bool ping)
    void set_ping_update_interval(int ping_update_interval)
    void set_font(char* font)
    void set_font_size(int font_size)
    void set_sound(bool sound)
    void set_sfx(int sfx)
    void set_music(int music)

def load_options(opts):
    set_name(opts.name)
    set_server(opts.server)
    set_port(opts.port)
    set_auto_assign_team(opts.auto_assign_team)
    set_width(opts.width)
    set_height(opts.height)
    set_fullscreen(opts.fullscreen)
    set_fov(opts.fov)
    set_sensitivity(opts.sensitivity)
    set_camera_speed(opts.camera_speed)
    set_invert_mouse(opts.invert_mouse)
    set_hud(opts.hud)
    set_diagnostic_hud(opts.diagnostic_hud)
    set_fps(opts.fps)
    set_ping(opts.ping)
    set_ping_update_interval(opts.ping_update_interval)
    set_font(opts.font)
    set_font_size(opts.font_size)
    set_sound(opts.sound)
    set_sfx(opts.sfx)
    set_music(opts.music)

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    void enumerate_sound_devices()

def show_sound_devices():
    enumerate_sound_devices()


"""
Game loop
"""
cdef extern from "./main.hpp" namespace "Main":
    void init()
    int run()
def init_game():
    init()
def run_game():
    run()


"""
Terrain Map
"""

cdef extern from "../c_lib/t_map/t_map.hpp" namespace "t_map":
    void init_t_map()
    void init_for_draw()

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

cdef extern from "./t_map/glsl/texture.hpp" namespace "t_map":
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

def init_terrain():
    print "Init Terrain Map"
    init_cube_properties()
    init_cube_side_texture()
    init_t_map()
    set_hud_cube_selector()
    init_for_draw()



