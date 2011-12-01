import opts
opts = opts.opts

import c_lib.c_lib_camera as cCamera
import c_lib.c_lib_input as cInput
from math import sin, cos, pi

base_dir = "./"

camera = None   # current active camera. Camera.load() will set this

def set_callback(callback):
    cCamera.callback = callback

class Camera(object):

    _local = ['camera', 'name', 'loaded']

    def __init__(self, x=0.0, y=0.0, z=0.0, x_angle=0.0, y_angle=0.0, name='', fov=85., first_person=False):
        self.camera = cCamera.Camera(first_person=first_person)
        self.camera.set_fov(fov)
        self.name = name
        self.x = x
        self.y = y
        self.z = z
        self.x_angle = x_angle
        self.y_angle = y_angle
        self.loaded = False

    def __getattribute__(self, name):
        if name in Camera._local:
            val = object.__getattribute__(self, name)
        else:
            try:
                cam = object.__getattribute__(self, 'camera')
                val = cam._getattribute__py(name)
            except AttributeError:
                val = object.__getattribute__(self, name)
        return val

    def __setattr__(self, k, v):
        if k in Camera._local:
            object.__setattr__(self, k, v)
        else:
            try:
                self.__dict__['camera']._setattr__py(k,v)
            except AttributeError, e:
                object.__setattr__(self, k, v)

    def input_update(self, t):
        dxa, dya = cInput.get_mouse_deltas(t)
        self.pan(*self._convert_mouse_deltas(dxa,dya))
        
    def _convert_mouse_deltas(self, dx, dy):
        invert = 1 if opts.invert_mouse else -1
        dx = (float(-dx) * opts.sensitivity) / 40000. # calibrated to sensitivity=100
        dy = (float(invert*dy) * opts.sensitivity) / 40000.
        return dx,dy

    def move_camera(self, dx, dy, dz):
        self.camera.move(dx,dy,dz)

    def pan(self, dx_angle, dy_angle):
        self.camera.pan(dx_angle, dy_angle)

    def pos(self, p=None):
        if p is None:
            return [self.x, self.y, self.z]
        else:
            self.x = p[0]
            self.y = p[1]
            self.z = p[2]
        
    def angles(self, a=None):
        if a is None:
            return [self.x_angle, self.y_angle]
        else:
            self.x_angle = a[0]
            self.y_angle = a[1]

    def load(self):
        if self.loaded:
            return
        global camera
        camera = self
        self.loaded = True
        self.camera.load()

    def unload(self):
        if not self.loaded:
            return
        self.loaded = False
        self.camera.unload()

    def forward(self):
        return self.camera.forward()
        
    def normal(self):
        return self.camera.normal()

    def world_projection(self):
        self.camera.world_projection()

    def hud_projection(self):
        self.camera.hud_projection()
