#scimport libc.stdlib

cdef extern from 'texture_loader.h':
    void _draw_loaded_hud_texture(int x, int y)
    void _load_hud_texture(char *file)

cdef extern from 'SDL_text.h':
    ctypedef struct SDL_Surface
    int _init_text()
    SDL_Surface* _create_text_surface(char* text,r,g,b,a)
    int _draw_text_surface(SDL_Surface* surface, int x, int y)
    int _free_text_surface(SDL_Surface* surface)
    int _draw_text(char* text, float x, float y, float height, float width, float depth)
    int _draw_text2(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a)

cdef extern from 'draw_functions.h':
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)

cdef extern from './hud/block_selector.h':
    void _draw_block_selector(int x, int y)
    void _load_block_selector_texture(char *file, int scale)

def init():
    _init_text()

def draw_text(text, x, y, height = 10., width = 10., depth = -.5):
    _draw_text(text, x, y, height, width, depth)

def create_text_surface(string):
    pass

def draw_loaded_hud_texture(x, y):
    _draw_loaded_hud_texture(x, y)

def load_hud_texture(file):
    _load_hud_texture(file)

def draw_block_selector(x, y):
    _draw_block_selector(x, y)

def load_block_selector_texture(file, scale=1):
    _load_block_selector_texture(file, scale)

class text:
#    cdef float x, y
#    cdef float height, width, depth
#    cdef int r,g,b,a
#    text = {}

    def __init__(self, text='', x=0, y=0, color=(255,255,255,255)):
        self.height = 10
        self.width = 10
        self.depth = -0.1
        if len(color) == 4:
            self.r, self.g, self.b, self.a = color
        else:
            self.r, self.g, self.b = color
            self.a = 255
        self.text = text
        self.x = x
        self.y = y

    def draw(self):
        assert self.text != None
        #_draw_text(self.text, self.x, self.y, self.height, self.width, self.depth)
        _draw_text2(self.text, self.x, self.y, self.height, self.width, self.depth, self.r, self.g, self.b, self.a)

    def _get_color(self):
        return (self.r, self.g, self.b, self.a)
    def _set_color(self, color):
        if len(color) == 4:
            self.r, self.g, self.b, self.a = color
        else:
            self.r, self.g, self.b = color
    def _del_color(self):
        del self.r, self.g, self.b, self.a

    color = property(_get_color, _set_color, _del_color, "text color")

class reticle:

    def __init__(self, file, x=0, y=0):
        self.x = x
        self.y = y
        load_hud_texture(file)

    def draw(self):
        draw_loaded_hud_texture(self.x, self.y)


class block_selector:

    def __init__(self, file, x=0, y=0, scale=1):
        self.x = x
        self.y = y
        self.scale = scale
        load_block_selector_texture(file, scale)

    def draw(self):
        draw_block_selector(self.x, self.y)
