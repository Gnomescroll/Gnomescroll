cdef extern from "./camera/camera.h":
    cdef struct Camera:
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

    int _world_projection(Camera* camera)
    int _hud_projection(Camera* camera)
    void _set_camera(Camera* c)
    void init_camera_c()

cdef class Camera:
    cdef Camera* camera

    def __init__(self, callback):
#        _set_camera(camera)
        self.set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
        self._set_projection(0.,0.,0.,0.,0.)
        self.callback = callback
            
    def set_aspect(self, float fov, float x_size, float y_size, float z_near, float z_far):
        cdef Camera* camera

        self.camera.fov = fov
        self.camera.x_size = x_size
        self.camera.y_size = y_size
        self.camera.z_near = z_near
        self.camera.z_far = z_far

    def set_projection(self, float x, float y, float z, float x_angle, float y_angle):
        print "SDL_Global set camera state"

        self.camera.x = x
        self.camera.y = y
        self.camera.z = z
        self.camera.x_angle = x_angle
        self.camera.y_angle = y_angle

    def world_projection(self):
        cdef Camera* c
        _world_projection(self.camera)
        if self.callback is not None:
            c = self.camera
            self.callback(c.x, c.y, c.z, c.xl, c.yl, c.zl, c.xu,c.yu, c.zu, c.ratio, c.fov)

    def hud_projection(self):
        _hud_projection(self.camera)
