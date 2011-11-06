
cimport libc.stdlib

## Texture Loader ##
cdef extern from "SDL.h":
    struct SDL_Surface:
        int w
        int h

cdef extern from "texture_loader.h":
    int _init_image_loader()
    SDL_Surface* _load_image(char *file)
    int _create_hud_texture(char *file) #deprecate
    int _create_block_texture(char *file) #deprecate
    int _create_texture(SDL_Surface* surface)

cdef SDL_Surface* load_image(char* file):
    cdef SDL_Surface* surface
    surface = _load_image(file)
    return surface

cdef create_texture(SDL_Surface* surface, type =None): #eventually support mippapped textures
    return _create_texture(surface)

## SDL functions ##
cdef extern from "SDL_functions.h":
    int _set_resolution(int xres, int yres, int fullscreen)
    int _init_video()
    int _del_video()
    int _swap_buffers()
    int _get_ticks()

def get_ticks():
    return _get_ticks()

## Settings ##
def set_resolution(xres, yres, fullscreen = 0):
    _set_resolution(xres, yres, fullscreen)
## Draw functions ##

cdef extern from "draw_functions.h":
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

## Window Properties ##
cdef class Window:
    cdef int w
    cdef int h

cdef class Texture:
    cdef int id
    cdef int w
    cdef int h
    cdef SDL_Surface* surface

    def __init__(Texture self, char * file):
        self.surface = load_image(file)
        self.w = self.surface.w
        self.h = self.surface.h
        self.id = _create_texture(self.surface)

    def draw(self, x0, y0, x1, y1, z=-0.5):
        _blit_sprite(self.id, x0, y0, x1, y1, z)

def init_video():
    print "Creating SDL OpenGL Window"
    _init_video()

def close_window():
    print "Deconstructing SDL OpenGL Window"
    _del_video()

def flip():
    _swap_buffers()

#particles
cdef extern from "particle_functions.h":
    int _init_particle_functions()
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
