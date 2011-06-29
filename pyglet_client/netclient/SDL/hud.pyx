cimport stdlib

cdef extern from 'SDL_text.h':
    ctypedef struct SDL_Surface
    int _init_text()
    SDL_Surface* _create_text_surface(char* text,r,g,b,a)
    int _draw_text_surface(SDL_Surface* surface, int x, int y)
    int _free_text_surface(SDL_Surface* surface)
    int _draw_text(char* text, float x, float y, float height, float width, float depth)

def init():
    _init_text()

def draw_text(text, x, y, height = 10., width = 10., depth = -.5):
    _draw_text(text, x, y, height, width, depth)

def create_text_surface(string):
    pass
