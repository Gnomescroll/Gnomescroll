cdef extern from "./camera/camera.h":
    cdef struct Camera2: #maybe public?
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

    int _world_projection(Camera2* camera2)
    int _hud_projection(Camera2* camera2)
    int _set_camera(Camera2* c)
    int _set_resolution_camera(int x, int y, int fullscreen)
    void init_camera_c()


def set_resolution(int x, int y, fullscreen):
    cdef int fs
    fs = int(bool(fullscreen))
    _set_resolution_camera(x,y,fs)

from libc.stdlib cimport malloc, free
# what the shit


#camera2
def set_aspect(float fov, float x_size, float y_size, float z_near, float z_far):
    cdef Camera2* camera2
    camera2 = camera2

    camera2.fov = fov
    camera2.x_size = x_size
    camera2.y_size = y_size
    camera2.z_near = z_near
    camera2.z_far = z_far

def set_projection(float x, float y, float z, float x_angle, float y_angle):
    cdef Camera2* camera2
    camera2 = camera2

    camera2.x = x
    camera2.y = y
    camera2.z = z
    camera2.x_angle = x_angle
    camera2.y_angle = y_angle

def world_projection():
    cdef Camera2* c
    _world_projection(camera2)
    global camera_callback
    if camera_callback != None:
        c = camera2
        camera_callback(c.x, c.y, c.z, c.xl, c.yl, c.zl, c.xu,c.yu, c.zu, c.ratio, c.fov)

def hud_projection():
    _hud_projection(camera2)

cdef Camera2 *camera2
#camera2 = <Camera2 *>malloc(sizeof(Camera2))
#_set_camera(camera2)
#set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
#set_projection(0.,0.,0.,0.,0.)

def init():
#    init_camera_c()
    global camera2
    camera2 = <Camera2 *>malloc(sizeof(Camera2))
    _set_camera(camera2)
    set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
    set_projection(0.,0.,0.,0.,0.)
