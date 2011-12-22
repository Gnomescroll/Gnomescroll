'''
SDL
'''
cdef extern from "./SDL/SDL_functions.h":
    int _set_resolution(int xres, int yres, int fullscreen)
    int _init_video()
    void _del_video()
    int _swap_buffers()
    int _get_ticks()

def flip():
    _swap_buffers()

def get_ticks():
    return _get_ticks()

def set_resolution(xres, yres, fullscreen = 0):
    _set_resolution(xres, yres, fullscreen)


'''
Draw functions
'''
cdef extern from "./SDL/draw_functions.h":
    int _draw_point(int r, int g,int b, float x0, float y0, float z0)
    int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1)
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)
    int _draw_rect(int r, int g, int b, float x, float y, float w, float h)
    int _draw_border_rect(int r, int g, int b, float x, float y, float w, float h)

def draw_line(int r, int g, int b, float x0, float y0, float z0, float x1, float y1, float z1):
    return _draw_line(r,g,b,x0,y0,z0,x1,y1,z1)

def draw_point(int r, int g, int b, float x0, float y0, float z0):
    return _draw_point(r,g,b,x0,y0,z0)

def draw_rect(int r, int g, int b, float x, float y, float w, float h):
    return _draw_rect(r,g,b, x,y, w,h)

def draw_border_rect(int r, int g, int b, float x, float y, float w, float h):
    return _draw_border_rect(r,g,b, x,y, w,h)


'''
Particles
'''

'''
cdef extern from "./SDL/particle_functions.h":
    int _draw_particle(int id, float size, float x, float y, float z)
    int _draw_particle2(int id, float size, float x, float y, float z)
    int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1)
    int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1)

def draw_particle(id,size, x,y,z):
    _draw_particle2(id, size, x, y, z)

def planar_laser(float x0, float y0, float z0, float x1, float y1, float z1):
    _planar_laser(x0, y0, z0, x1, y1, z1)

def planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1):
    _planar_laser2(density, width, x0, y0, z0, x1, y1, z1)
'''
