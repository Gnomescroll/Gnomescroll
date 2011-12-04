cdef extern from "./camera/camera.hpp":
    cdef cppclass CCamera:
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

        int current()

        int world_projection()
        int hud_projection()

        void pan(float dx, float dy)
        void move(float dx, float dy, float dz)
        void set_aspect(float fov, float z_near, float z_far)
        void set_projection(float x, float y, float z, float x_angle, float y_angle)
        void set_dimensions()   # uses SDL_functions' xres & yres properties
        void set_fov(float fov)
        
    void set_camera(CCamera* c)
    void set_camera_first_person(int first_person)
    CCamera* get_available_camera()

camera_properties = [
    'fov',
    'x_size', 'y_size',
    'z_near', 'z_far',
    'x', 'y', 'z',
    'x_angle', 'y_angle',
    'xl', 'yl', 'zl',
    'xu', 'yu', 'zu',
    'ratio'
]

callback = None

cdef class Camera(object):
    cdef CCamera* camera
    cdef int active
    cdef int first_person

    def __init__(self, int first_person):
        cdef CCamera* cam

        cam = get_available_camera();
        if cam == NULL:
            print "Cython camera init -- Camera is null"
            raise ValueError

        self.camera = cam
        self.active = 0
        if self.camera.current():
            print 'Cython camera is current camera'
            self.active = 1

        self.camera.set_dimensions()

        self.first_person = int(first_person)

    def load(self):
        if self.active == 0:
            set_camera(self.camera)
            set_camera_first_person(self.first_person)
            self.active = 1

    def unload(self):
        self.active = 0

    def set_fov(self, float fov):
        self.camera.set_fov(fov)
            
    def set_aspect(self, float fov, float z_near, float z_far):
        self.camera.set_aspect(fov, z_near, z_far)

    def set_projection(self, float x, float y, float z, float x_angle, float y_angle):
        self.camera.set_projection(x,y,z, x_angle, y_angle)

    def pan(self, float dx, float dy):
        self.camera.pan(dx,dy)

    def move(self, float dx, float dy, float dz):
        self.camera.move(dx, dy, dz)

    def forward(self):
        return [self.camera.xl, self.camera.yl, self.camera.zl]
        
    def normal(self):
        return [self.camera.xu, self.camera.yu, self.camera.zu]
        
    def world_projection(self):
        cdef CCamera* c
        self.camera.world_projection()
        if callback is not None:
            c = self.camera
            callback(c.x, c.y, c.z, c.xl, c.yl, c.zl, c.xu,c.yu, c.zu, c.ratio, c.fov)

    def hud_projection(self):
        self.camera.hud_projection()

    def _getattribute__py(self, name):
        if name == 'x':
            return self.camera.x
        elif name == 'y':
            return self.camera.y
        elif name == 'z':
            return self.camera.z
        elif name == 'xl':
            return self.camera.xl
        elif name == 'yl':
            return self.camera.yl
        elif name == 'zl':
            return self.camera.zl
        elif name == 'xu':
            return self.camera.xu
        elif name == 'yu':
            return self.camera.yu
        elif name == 'zu':
            return self.camera.zu
            
        elif name == 'x_angle':
            return self.camera.x_angle
        elif name == 'y_angle':
            return self.camera.y_angle

        elif name == 'ratio':
            return self.camera.ratio
        elif name == 'fov':
            return self.camera.fov
        elif name == 'x_size':
            return self.camera.x_size
        elif name == 'y_size':
            return self.camera.y_size

        elif name == 'z_near':
            return self.camera.z_near
        elif name == 'z_far':
            return self.camera.z_far

        raise AttributeError

    def _setattr__py(self, k, float v):
        if k == 'x':
            self.camera.x = v
        elif k == 'y':
            self.camera.y = v
        elif k == 'z':
            self.camera.z = v
        elif k == 'xl':
            self.camera.xl = v
        elif k == 'yl':
            self.camera.yl = v
        elif k == 'zl':
            self.camera.zl = v
        elif k == 'xu':
            self.camera.xu = v
        elif k == 'yu':
            self.camera.yu = v
        elif k == 'zu':
            self.camera.zu = v
        elif k == 'x_angle':
            self.camera.x_angle = v
        elif k == 'y_angle':
            self.camera.y_angle = v

        elif k == 'ratio':
            self.camera.ratio = v
        elif k == 'fov':
            self.camera.fov = v
        elif k == 'x_size':
            self.camera.x_size = v
        elif k == 'y_size':
            self.camera.y_size = v

        elif k == 'z_near':
            self.camera.z_near = v
        elif k == 'z_far':
            self.camera.z_far = v

        raise AttributeError

"""
Skybox
"""
cdef extern from "./skybox/skybox.hpp" namespace "Skybox":
    void render()
    void load()

def load_skybox():
    load()

def render_skybox():
    render()
