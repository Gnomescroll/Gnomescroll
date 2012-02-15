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

"""
Font
[hud]
-- The hud depdnency is only the calling of the init() method
-- No python is dependent on this code here; its mostly doing parsing
-- It does need to add font set metadata, however
"""
#cdef extern from "./hud/font.hpp" namespace "HudFont":
#    int init_font()

#def init_font_set():
#    init_font()

#    void add_glyph(
#        int c,
#        float x, float y,
#        float xoff, float yoff,
#        float w, float h,
#        float xadvance
#    )
#    void set_missing_character(int cc)

#import os.path

#class Font:

#    font_path = "./media/fonts/"
#    font_ext = ".fnt"
#    png_ext = "_0.png"
#    missing_character = '?'
#    glyphs = {}
#    info = {}
#    font = None

#    ready = False
    
#    @classmethod
#    def init(cls):
#        if not os.path.exists(cls.font_path):
#            print "ERROR c_lib_fonts.pyx :: cannot find font path %s" % (cls.font_path,)
#            cls.ready = False
#            return

#        import opts
#        cls.font = cls(opts.opts.font)
#        cls.font.parse()
#        cls.font.load()

#    def __init__(self, fn):
#        self.fontfile = fn
#        self.pngfile = ''
#        self.process_font_filename()
        
#    def process_font_filename(self):
#        fn = self.fontfile
#        fn = fn.split('.')[0]
#        fn += self.font_ext
#        fn = self.font_path + fn
#        self.fontfile = fn
#        if not os.path.exists(self.fontfile):
#            print "ERROR c_lib_fonts.pyx :: cannot find font file %s in %s" % (fn, self.font_path,)
#            self.ready = False
#            return
#        self.ready = True
            
#    def parse(self):
#        png = ""
        
#        # parse .fnt
#        with open(self.fontfile) as f:
#            lines = f.readlines()
#            for line in lines:
#                line = line.strip()
#                tags = line.split()
#                name = tags[0]
#                tags = dict(map(lambda a: a.split('='), tags[1:]))

#                if name == 'page':
#                    png = tags['file'].strip('"')
#                elif name == 'info':
#                    self.info.update(tags)
#                    print "Font: %s" % (line,)
#                elif name == 'common':
#                    self.info.update(tags)
#                    print "Font: %s" % (line,)
#                elif name == 'chars':
#                    print '%s characters in font set' % (tags['count'],)
#                elif name == 'char':
#                    self.glyphs[int(tags['id'])] = tags

#        # process png filename
#        if not png:
#            png = self.fontfile + self.png_ext
#        fp_png = self.font_path + png
#        if not os.path.exists(fp_png):
#            print "ERROR c_lib_fonts.pyx :: cannot find font png file %s in %s" % (fp_png, self.font_path,)
#            self.ready = False
#            return
#        self.pngfile = fp_png

#        self.clean_glyphs()
#        self.missing_character_available()

#    def add_glyphs_to_c(self):
#        for char_code, glyph in self.glyphs.items():
#            x = float(glyph['x'])
#            y = float(glyph['y'])
#            xoff = float(glyph['xoffset'])
#            yoff = float(glyph['yoffset'])
#            w = float(glyph['width'])
#            h = float(glyph['height'])
#            xadvance = float(glyph['xadvance'])
#            add_glyph(char_code, x, y, xoff, yoff, w,h, xadvance)

#            if char_code == ord(' '):
#                add_glyph(ord('\t'), x,y, xoff, yoff, w,h, xadvance)
                
#    def clean_glyphs(self):
#        for kc, glyph in self.glyphs.items():
#            for k,v in glyph.items():
#                try:
#                    glyph[k] = int(glyph[k])
#                except ValueError:
#                    pass

#    def missing_character_available(self):
#        cc = ord(self.missing_character)
#        if cc not in self.glyphs:
#            print "ERROR Missing character placeholder %s is not a known glyph" % (self.missing_character,)
#            self.ready = False
#            return False
#        set_missing_character(cc)
#        return True
        
#    def load(self):
#        if not load_font(self.pngfile):
#            self.ready = False
#            return
#        self.add_glyphs_to_c()
#        self.ready = True

#cdef extern from "./hud/font_loader.hpp":
#    void load_fonts()
#def start_font():
#    load_fonts()

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

