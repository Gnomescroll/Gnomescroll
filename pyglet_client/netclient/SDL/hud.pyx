cimport stdlib

cdef extern from 'SDL_text.h':
    ctypedef struct SDL_Surface
    int _init_text()
    SDL_Surface* _create_text_surface(char* text,r,g,b,a)
    int _draw_text_surface(SDL_Surface* surface, int x, int y)
    int _free_text_surface(SDL_Surface* surface)
    int _draw_text(char* text, float x, float y)

def init():
    _init_text()

def draw_text(text, x, y):
    _draw_text(text, x, y)

def create_text_surface(string):
    pass

cdef class Text:
    cdef SDL_Surface* surface
    cdef int x
    cdef int y

    def __init__(self, text, r=0, g=255, b=0, a=0):
        self.surface = _create_text_surface(text,r,g,b,a)
        self.x = 50
        self.y = 50
        print "hud.pyx: Font surface created"

    def __del__(self):
        self.free()
        print "hud.pyx: Font surface deconstructed"

    cpdef draw(self):
        _draw_text_surface(self.surface, self.x, self.y)

    cpdef free(self):
        _free_text_surface(self.surface)
