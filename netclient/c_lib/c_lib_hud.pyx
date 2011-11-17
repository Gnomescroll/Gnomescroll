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
    int draw_text(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a)
    int load_font(char* fontfile)

    void start_text_draw(int r, int g, int b, int a)
    void end_text_draw()
    void blit_glyph(
        float tex_x_min, float tex_x_max,
        float tex_y_min, float tex_y_max,
        float screen_x_min, float screen_x_max,
        float screen_y_min, float screen_y_max,
        float depth
    )

class Text:

    def __init__(self, text='', x=0, y=0, color=(255,255,255,255)):
        self.height = 10
        self.width = 10
        self.depth = -0.1
        self.color = list(color)
        if len(self.color) == 3:
            self.color.append(255) # default alpha

        self.text = text
        self.x = x
        self.y = y

    def draw(self):
        r,g,b,a  = self.color
#        draw_text(self.text, self.x, self.y, self.height, self.width, self.depth, r,g,b,a)
        Font.font.draw(self.text, self.x, self.y, self.depth, self.color)

''' Font '''
import os.path
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

    def clean_glyphs(self):
        for kc, glyph in self.glyphs.items():
            for k,v in glyph.items():
                try:
                    glyph[k] = int(glyph[k])
                except ValueError:
                    pass
        
    def load(self):
        if not load_font(self.pngfile):
            self.ready = False
        self.ready = True

    def draw_test_glyph(self):
        cursor_x=cursor_y=0
        cc = ord("A")
        data = self.glyphs[cc]
        start_text_draw(255,20,20,255)
        tx_min = data['x'] / 256.
        tx_max = (data['x'] + data['width']) / 256.
        ty_max = (256 - data['y']) / 256.
        ty_min = (256 - data['y'] - data['height']) / 256.
#        tx_min = 0.
#        tx_max = 1.
#        ty_min = 0.
#        ty_max = 1.
        # surface quad coordinates
        sx_min = 660 + data['xoffset']
        sx_max = 660 + cursor_x + data['width']
        sy_min = 440 + data['height']
        sy_max = 440 + cursor_y + data['yoffset']
#        sx_min = 640
#        sx_max = 640+256
#        sy_max = 400
#        sy_min = 400+256
        print tx_min, tx_max, ty_min, ty_max
        blit_glyph(tx_min, tx_max, ty_min, ty_max, sx_min, sx_max, sy_min, sy_max, 0.1)

        end_text_draw()
        
    def draw(self, text, x, y, depth, color):
        if not self.ready:
            print "Cannot draw font. Font not ready"
            return

        self.draw_test_glyph()
#        return

        r,g,b,a = color
        start_text_draw(r,g,b,a);

        cursor_x = cursor_y = 0
        for c in text:
            cc = ord(c)

            if cc == 10:
                cursor_y += 10
                continue
            
            if cc not in self.glyphs:
                print "Character unknown: %s" % (c,)
                cc  = ord(self.missing_character)
                
            data = self.glyphs[cc]

            # font texture coordinates
            tx_min = data['x'] / 256.
            tx_max = (data['x'] + data['width'] ) / 256.
            ty_max = data['y'] / 256.
            ty_min = (data['y'] - data['height']) / 256.

            # surface quad coordinates
            sx_min = x + cursor_x + data['xoffset']
            sx_max = sx_min + data['width']
            sy_min = y - cursor_y - data['yoffset']
            sy_max = sy_min - data['height']

            # copy glyph
            blit_glyph(tx_min, tx_max, ty_min, ty_max, sx_min, sx_max, sy_min, sy_max, depth)

            # advance quad cursor
            cursor_x += data['xadvance']
    
        end_text_draw()
