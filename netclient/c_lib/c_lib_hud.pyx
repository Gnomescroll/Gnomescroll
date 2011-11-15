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
    int _init_text()
    int draw_text(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a)

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
        draw_text(self.text, self.x, self.y, self.height, self.width, self.depth, r,g,b,a)

