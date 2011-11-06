cdef extern from "./camera/camera.h":
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

cdef extern from "./camera/camera.h":
    int _world_projection(Camera* camera)
    int _hud_projection(Camera* camera)
    int _set_camera(Camera* c)
    int _set_resolution_camera(int x, int y, int fullscreen)
    void init_camera_c()


def set_resolution(int x, int y, fullscreen):
    cdef int fs
    fs = int(bool(fullscreen))
    _set_resolution_camera(x,y,fs)

from libc.stdlib cimport malloc, free
# what the shit


#camera
def set_aspect(float fov, float x_size, float y_size, float z_near, float z_far):
    cdef Camera* camera
    camera = camera

    camera.fov = fov
    camera.x_size = x_size
    camera.y_size = y_size
    camera.z_near = z_near
    camera.z_far = z_far

def set_projection(float x, float y, float z, float x_angle, float y_angle):
    cdef Camera* camera
    camera = camera

    camera.x = x
    camera.y = y
    camera.z = z
    camera.x_angle = x_angle
    camera.y_angle = y_angle

def world_projection():
    cdef Camera* c
    _world_projection(camera)
    global camera_callback
    if camera_callback != None:
        c = camera
        camera_callback(c.x, c.y, c.z, c.xl, c.yl, c.zl, c.xu,c.yu, c.zu, c.ratio, c.fov)

def hud_projection():
    _hud_projection(camera)

cdef Camera *camera
#camera = <Camera *>malloc(sizeof(Camera))
#_set_camera(camera)
#set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
#set_projection(0.,0.,0.,0.,0.)

def init():
    init_camera_c()
    global camera
    camera = <Camera *>malloc(sizeof(Camera))
    _set_camera(camera)
    set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
    set_projection(0.,0.,0.,0.,0.)
