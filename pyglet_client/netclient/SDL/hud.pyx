
cdef extern from 'SDL_text.h':
    int _init_text()

def init_text():
    _init_text()
