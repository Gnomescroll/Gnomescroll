cimport stdlib

cdef extern from 'SDL_text.h':
    ctypedef struct SDL_Surface
    int _init_text()
    SDL_Surface* _create_text_surface(char* text,r,g,b,a)
    int _draw_text_surface(SDL_Surface* surface, int x, int y)
    int _free_text_surface(SDL_Surface* surface)
    int _draw_text(char* text, float x, float y, float height, float width, float depth)
    int _draw_text2(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a)

def init():
    _init_text()

def draw_text(text, x, y, height = 10., width = 10., depth = -.5):
    _draw_text(text, x, y, height, width, depth)

def create_text_surface(string):
    pass

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
    
        
    
        
