'''
HUD textures
'''

cdef extern from "./SDL/texture_loader.h":
    struct Texture:
        int w
        int h
        int tex

    Texture _load_image_create_texture(char *file)

cdef extern from "./SDL/draw_functions.h":
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)

cdef class CyTexture:
    cdef int texture
    cdef int w
    cdef int h

    def __init__(Texture self, char * file):
        cdef Texture tex
        tex = _load_image_create_texture(file)
        self.w = tex.w
        self.h = tex.h
        self.texture = tex.tex

    def draw(self, x0, y0, x1, y1, z=-0.5):
        _blit_sprite(self.texture, x0, y0, x1, y1, z)


'''
Reticle
'''
cdef class Reticle(CyTexture):
    cdef float x0
    cdef float y0
    cdef float x1
    cdef float y1

    def __init__(Reticle self, char* file, int window_width, int window_height):
        CyTexture.__init__(self, file)

        center_x = window_width / 2.
        center_y = window_height / 2.

        self.x0 = center_x - (self.w / 2.)
        self.y0 = center_y - (self.h / 2.)
        self.x1 = self.x0 + self.w
        self.y1 = self.y0 + self.h

    def draw(self):
        _blit_sprite(self.texture, self.x0, self.y0, self.x1, self.y1, 0.)


'''
HUD Cube Selector
'''

cdef extern from "./hud/cube_selector.hpp":
    void set_cube_selector_property(int pos, int cube_id, int tex_id)
    void draw_cube_selector(float x, float y, float size, int mode)
    void set_active_cube_id(int id)
    void set_active_cube_pos(int pos)
    int get_active_cube_id()
    int get_active_cube_pos()


cdef class CubeSelector:

    cdef float x
    cdef float y
    cdef float size
    cdef int mode

    @classmethod
    def load_cube_properties(cls, int pos, int cube_id, int tex_id):
        set_cube_selector_property(pos, cube_id, tex_id)

    def __init__(self, float x, float y, int active_pos=0):
        self.x = x
        self.y = y
        
    def draw(self):
        draw_cube_selector(self.x, self.y, 1., 0)

    def set(self, int pos):
        set_active_cube_pos(pos)

    def set_id(self, int id):
        set_active_cube_id(id)

    def active(self):
        return get_active_cube_pos()

    def active_id(self):
        return get_active_cube_id()


'''
Inventory
'''

cdef extern from "./hud/inventory.hpp":
    int draw_inventory(float x, float y)

cdef class Inventory:
    cdef float x
    cdef float y

    def __init__(self, float x, float y):
        self.x = x
        self.y = y

    def draw(self):
        draw_inventory(self.x, self.y)


'''
Text
'''

cdef extern from './hud/text.h':
    int load_font(char* fontfile)

    void start_text_draw()
    void set_text_color(int r, int g, int b, int a)
    void end_text_draw()

    void blit_glyph(
        float tex_x_min, float tex_x_max,
        float tex_y_min, float tex_y_max,
        float screen_x_min, float screen_x_max,
        float screen_y_min, float screen_y_max,
        float depth
    )

    void add_glyph(
        int c,
        float x, float y,
        float xoff, float yoff,
        float w, float h,
        float xadvance
    )
    void set_missing_character(int cc)
    void draw_text(char* t, int len, float x, float y, float depth)

class Text(object):

    def __init__(self, text='', x=0, y=0, color=(255,255,255,255)):
        self.height = 10
        self.width = 10
        self.depth = -0.1
        self.color = list(color)
        if len(self.color) == 3:
            self.color.append(255) # default alpha

        self.text = text
        self.text_len = len(text)
        self.x = x
        self.y = y

    def __setattr__(self, k, v):
        if k == 'text':
            object.__setattr__(self, "text_len", len(v))
        object.__setattr__(self, k, v)

    def draw(self):
        r,g,b,a = self.color
        set_text_color(r,g,b,a)
        draw_text(self.text, self.text_len, self.x, self.y, self.depth)
        
''' Font '''
import os.path
import random
import string

class Font:

    font_path = "./media/fonts/"
    font_ext = ".fnt"
    png_ext = "_0.png"
    missing_character = '?'
    glyphs = {}
    info = {}
    font = None

    ready = False
    
    @classmethod
    def init(cls):
        if not os.path.exists(cls.font_path):
            print "ERROR c_lib_fonts.pyx :: cannot find font path %s" % (cls.font_path,)
            cls.ready = False
            return

        import opts
        cls.font = cls(opts.opts.font)
        cls.font.parse()
        cls.font.load()

    def __init__(self, fn):
        self.fontfile = fn
        self.pngfile = ''
        self.process_font_filename()
#        self._gen_stress()
        
    def process_font_filename(self):
        fn = self.fontfile
        fn = fn.split('.')[0]
        fn += self.font_ext
        fn = self.font_path + fn
        self.fontfile = fn
        if not os.path.exists(self.fontfile):
            print "ERROR c_lib_fonts.pyx :: cannot find font file %s in %s" % (fn, self.font_path,)
            self.ready = False
            return
        self.ready = True
            
    def parse(self):
        png = ""
        
        # parse .fnt
        with open(self.fontfile) as f:
            lines = f.readlines()
            for line in lines:
                line = line.strip()
                tags = line.split()
                name = tags[0]
                tags = dict(map(lambda a: a.split('='), tags[1:]))

                if name == 'page':
                    png = tags['file'].strip('"')
                elif name == 'info':
                    self.info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'common':
                    self.info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'chars':
                    print '%s characters in font set' % (tags['count'],)
                elif name == 'char':
                    self.glyphs[int(tags['id'])] = tags

        # process png filename
        if not png:
            png = self.fontfile + self.png_ext
        fp_png = self.font_path + png
        if not os.path.exists(fp_png):
            print "ERROR c_lib_fonts.pyx :: cannot find font png file %s in %s" % (fp_png, self.font_path,)
            self.ready = False
            return
        self.pngfile = fp_png

        self.clean_glyphs()
        self.missing_character_available()

    def add_glyphs_to_c(self):
        for char_code, glyph in self.glyphs.items():
            x = float(glyph['x'])
            y = float(glyph['y'])
            xoff = float(glyph['xoffset'])
            yoff = float(glyph['yoffset'])
            w = float(glyph['width'])
            h = float(glyph['height'])
            xadvance = float(glyph['xadvance'])
            add_glyph(char_code, x, y, xoff, yoff, w,h, xadvance)
                
    def clean_glyphs(self):
        for kc, glyph in self.glyphs.items():
            for k,v in glyph.items():
                try:
                    glyph[k] = int(glyph[k])
                except ValueError:
                    pass

    def missing_character_available(self):
        cc = ord(self.missing_character)
        if cc not in self.glyphs:
            print "ERROR Missing character placeholder %s is not a known glyph" % (self.missing_character,)
            self.ready = False
            return False
        set_missing_character(cc)
        return True
        
    def load(self):
        if not load_font(self.pngfile):
            self.ready = False
            return
        self.add_glyphs_to_c()
        self.ready = True

    def _gen_stress(self):
        num = 4096
        self.stressers = []
        for i in range(num):
            s = random.choice(string.letters)
            x = float(random.randrange(0, 1280))
            y = float(random.randrange(0, 800))
            color = (random.randrange(0,256),random.randrange(0,256),random.randrange(0,256),random.randrange(0,256))
            self.stressers.append((s, x,y,color))
      
    def stress_test(self):
        set_text_color(100,100,100,255)
        for s,x,y,color in self.stressers:
            r,g,b,a = color
            set_text_color(r,g,b,a)
            draw_text(s, 1, x,y,0.1)

    def start(self):
        start_text_draw()
        
    def end(self):
        end_text_draw()

    def set_color(self, color):
        r,g,b,a = color
        set_text_color(r,g,b,a)

"""Map"""
cdef extern from "hud/map.hpp" namespace "HudMap":
    void update_map()
    void draw_map()

class Map:
    @classmethod
    def update(cls):
        update_map()
    @classmethod
    def draw(cls):
        draw_map()

"""Equipment Panel"""
cdef extern from "hud/equipment.hpp" namespace "HudEquipment":
    void draw_equipment(int slot)
    void set_slot_icon(int slot, int icon_id)

class Equipment:
    @classmethod
    def draw(cls, int slot):
        draw_equipment(slot)

    @classmethod
    def set_equipment_icon(cls, int slot, int icon_id):
        set_slot_icon(slot, icon_id)
