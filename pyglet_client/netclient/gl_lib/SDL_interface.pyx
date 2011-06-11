'''
cdef extern from "camera.c":
    struct Camera:
        float x_size, y_size, z_near, z_far
        float x,y,z, x_angle,y_angle
    _camera_projection(Camera c)
'''

#cdef extern from "SDL.h":

## Camera.c ##
cdef struct Camera: #maybe public?
    float fov, x_size, y_size, z_near, z_far
    float x,y,z, x_angle,y_angle

cdef extern int _world_projection(Camera camera)
cdef extern int _hud_projection(Camera camera)

cpdef int init_video():
    return _init_video()

## End Camera.c ##


## SDL functions ##

cdef extern int _init_video()
cdef extern int _del_video()
cdef extern int _swap_buffers()

## Draw functions ##

cdef extern int _draw_point(int r, int g,int b, float x0, float y0, float z0)
cdef extern int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1)

def draw_line(int r, int g, int b, float x0, float y0, float z0, float x1, float y1, float z1):
    return _draw_line(r,g,b,x0,y0,z0,x1,y1,z1)

def draw_point(int r, int g, int b, float x0, float y0, float z0):
    return _draw_point(r,g,b,x0,y0,z0)

## input.c
cdef extern int _init_input()
cdef extern int _get_key_state()
cdef extern int _get_key_event()
cdef extern int _set_text_entry_mode(int n)

def init_input():
    _init_input()
def get_key_state():
    temp = _get_key_state()

def get_key_event():
    temp = _get_key_event()

def set_text_entry_mode(int n):
    temp = _set_text_entry_mode(n)

## Window Properties ##
'''
#window propertiesSDL.
cdef public class Window:
    cdef int x_size
    cdef int y_size
'''


cdef class Global:
    cdef Camera camera
#    cdef Window window

    #make field of view adjustable!
    def init(self):
        print "Creating SDL OpenGL Window"
        self.set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
        self.set_projection(0.,0.,0.,0.,0.)
        _init_video()
        #input init
        init_input()

    def close_window(self):
        print "Deconstructing SDL OpenGL Window"
        _del_video()

    def swap_buffers(self):
        _swap_buffers()

    #camera
    def set_aspect(self, float fov, float x_size, float y_size, float z_near, float z_far):
        self.camera.fov = fov
        self.camera.x_size = x_size
        self.camera.y_size = y_size
        self.camera.z_near = z_near
        self.camera.z_far = z_far

    def set_projection(self, float x, float y, float z, float x_angle, float y_angle):
        self.camera.x = x
        self.camera.y = y
        self.camera.z = z
        self.camera.x_angle = x_angle
        self.camera.y_angle = y_angle

    def world_projection(Global self):
        _world_projection(self.camera)

    def hud_projection(Global self):
        _hud_projection(self.camera)


class MouseEventHandler:
    def __init__(self):
        pass
### init

ctypedef struct MouseState:
    int x
    int y
    int dx
    int dy

ctypedef struct MouseEvent:
    int x
    int y
    int button1
    int button2
    int button3
    int button4
    int button5

### call backs
cdef extern from "input.h":
    ctypedef void (*key_state_func)()
    ctypedef void (*key_event_func)(char key)
    ctypedef void (*mous_movement_func)(MouseState ms)
    ctypedef void (*mouse_event_func)(MouseEvent me)

#int _key_state_callback(key_state_func user_func);  ///no idea how to do this yet
#int _key_event_callback(key_event_func user_func, char key);
#int _mouse_movement_callback(key_movement_func user_func, MouseState ms);
#int _mouse_event_callback(mouse_movement_func user_func, MouseEvent me);

cdef void key_state_func():
    pass

cdef void key_state_callback(char key):
    pass

cdef void mouse_movement_func(MouseState ms):
    pass

cdef void mouse_event_func(MouseEvent me):
    pass

SDL_global = Global()
