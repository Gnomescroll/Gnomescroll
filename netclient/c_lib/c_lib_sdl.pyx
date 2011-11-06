cimport libc.stdlib

## Camera.c ##
cdef extern from "./SDL/camera.h":
    cdef struct Camera: #maybe public?
        float fov
        float x_size
        float y_size
        float z_near
        float z_far
        float x
        float y
        float z
        float x_angle
        float y_angle
        float xl, yl, zl
        float xu, yu, zu
        float ratio

cdef extern from "./SDL/camera.h":
    int _world_projection(Camera* camera)
    int _hud_projection(Camera* camera)
    int _set_camera(Camera* c)

## Texture Loader ##
cdef extern from "./SDL/SDL.h":
    struct SDL_Surface:
        int w
        int h

cdef extern from "./SDL/texture_loader.h":
    int _init_image_loader()
    SDL_Surface* _load_image(char *file)
    int _create_block_texture(char *file) #deprecate
    int _create_texture(SDL_Surface* surface)

cdef int testt():
    cdef int i
    i = _init_image_loader()
    return i

cdef SDL_Surface* load_image(char* file):
    cdef SDL_Surface* surface
    surface = _load_image(file)
    return surface

cdef create_texture(SDL_Surface* surface, type =None): #eventually support mippapped textures
    return _create_texture(surface)

## SDL functions ##
cdef extern from "./SDL/SDL_functions.h":
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
cdef extern from "./SDL/draw_functions.h":
    int _draw_point(int r, int g,int b, float x0, float y0, float z0)
    int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1)
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)
    int _draw_rect(int r, int g, int b, float x, float y, float w, float h)
    int _draw_border_rect(int r, int g, int b, float x, float y, float w, float h)
    #int _bind_VBO(Quad* quad_list,int v_num)

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

class Textures:
    hud_tex = None

    def init(self):
        print "Initing Textures"
        self.hud_tex = Texture("./media/texture/target.png")

from libc.stdlib cimport malloc, free

camera_callback = None

cdef class Global:
    cdef Camera* camera

    #make field of view adjustable!
    def __init__(self):
        cdef Camera *camera
        camera = <Camera *>malloc(sizeof(Camera))
        self.camera = camera
        _set_camera(camera)
        self.set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
        self.set_projection(0.,0.,0.,0.,0.)

    def init(self):
        print "Creating SDL OpenGL Window"
        _init_video()

    def close_window(self):
        print "Deconstructing SDL OpenGL Window"
        _del_video()

    def flip(self):
        _swap_buffers()

    #camera
    def set_aspect(self, float fov, float x_size, float y_size, float z_near, float z_far):
        cdef Camera* camera
        camera = self.camera

        camera.fov = fov
        camera.x_size = x_size
        camera.y_size = y_size
        camera.z_near = z_near
        camera.z_far = z_far

    def set_projection(self, float x, float y, float z, float x_angle, float y_angle):
        cdef Camera* camera
        camera = self.camera

        camera.x = x
        camera.y = y
        camera.z = z
        camera.x_angle = x_angle
        camera.y_angle = y_angle

    def world_projection(self):
        cdef Camera* c
        _world_projection(self.camera)
        global camera_callback
        if camera_callback != None:
            c = self.camera
            camera_callback(c.x, c.y, c.z, c.xl, c.yl, c.zl, c.xu,c.yu, c.zu, c.ratio, c.fov)

    def hud_projection(self):
        _hud_projection(self.camera)

### init

SDL_global = Global()

#particles

cdef extern from "./SDL/particle_functions.h":
    int _init_particle_functions()
    int _draw_particle(int id, float size, float x, float y, float z)
    int _draw_particle2(int id, float size, float x, float y, float z)
    int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1)
    int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1)

def init_particles():
    _init_particle_functions()


def draw_particle(id,size, x,y,z):
    _draw_particle2(id, size, x, y, z)


def planar_laser(float x0, float y0, float z0, float x1, float y1, float z1):
    _planar_laser(x0, y0, z0, x1, y1, z1)

def planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1):
    _planar_laser2(density, width, x0, y0, z0, x1, y1, z1)

### Hud

#cdef extern from "./SDL/./hud/block_selector.h":
#    int _init_block_selector()

#def init_block_selector():
#    _init_block_selector()

"""
HUD.pyx
"""

cdef extern from './SDL/texture_loader.h':
    void _draw_loaded_hud_texture(int x, int y)
    void _load_hud_texture(char *file)



cdef extern from './SDL/SDL_text.h':
#    ctypedef struct SDL_Surface
    int _init_text()
    SDL_Surface* _create_text_surface(char* text,r,g,b,a)
    int _draw_text_surface(SDL_Surface* surface, int x, int y)
    int _free_text_surface(SDL_Surface* surface)
    int _draw_text(char* text, float x, float y, float height, float width, float depth)
    int _draw_text2(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a)

cdef extern from './SDL/draw_functions.h':
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)

cdef extern from './SDL/block_selector.h':
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

'''
New
'''
def set_projection(float x, float y, float z, float x_angle, float y_angle):
    cdef Camera* camera
    camera = camera

    camera.x = x
    camera.y = y
    camera.z = z
    camera.x_angle = x_angle
    camera.y_angle = y_angle

