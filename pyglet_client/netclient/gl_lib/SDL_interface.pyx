'''
cdef extern from "camera.c":
    struct Camera:
        float x_size, y_size, z_near, z_far
        float x,y,z, x_angle,y_angle
    _camera_projection(Camera c)
'''


## Camera.c ##
ctypedef struct Camera:
    float fov, x_size, y_size, z_near, z_far
    float x,y,z, x_angle,y_angle

cdef extern _world_projection(Camera camera)
cdef extern _hud_projection(Camera camera)

## End Camera.c ##


## SDL functions ##

cdef extern int _init_video()
cdef extern int _del_video()


'''
#window properties
cdef class Window:
    cdef int x_size
    cdef int y_size
'''

cdef class Global:
    cdef Camera camera
#    cdef Window window

    def __init__(self):
        print "Creating SDL OpenGL Window"
        self.set_aspect(0,800, 600, 0.1, 1000)
        self.set_projection(0,0,0,0,0)
        _init_video()

    def close_window(self):
        print "Deconstructing SDL OpenGL Window"
        _del_video()

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

cpdef int init_video():
    return _init_video()
