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

    def __init__(self, x=0.0, y=0.0, z=0.0, x_angle=0.0, y_angle=0.0, name=''):
        self.camera = cCamera.Camera()
        #self.camera.set_projection(x,y,z, x_angle, y_angle)

        self.name = name
        self.x = x
        self.y = y
        self.z = z
        self.x_angle = x_angle
        self.y_angle = y_angle
        self.loaded = False

    def __getattribute__(self, name):
        try:
            cam = object.__getattribute__(self, 'camera')
            val = cam._getattribute__py(name)
        except AttributeError:
            val = object.__getattribute__(self, name)
        return val

    def __setattr__(self, k, v):
        if k not in cCamera.camera_properties:
            object.__setattr__(self, k, v)
            return

        try:
            self.__dict__['camera']._setattr__py(k,v)
        except AttributeError, e:
            print e
            object.__setattr__(self, k, v)

    def input_update(self):
        dxa, dya = cInput.get_mouse_deltas()
        self.pan(*self._convert_mouse_deltas(dxa,dya))
        
    def _convert_mouse_deltas(self, dx, dy):
        invert = 1 if opts.invert_mouse else -1
        dx = (float(-dx) * opts.sensitivity) / 40000. # calibrated to sensitivity=100
        dy = (float(invert*dy) * opts.sensitivity) / 40000.
        return dx,dy

    def move_camera(self, dx, dy, dz):
        #dx delta
        self.x += dx*cos(self.x_angle * pi)
        self.y += dx*sin(self.x_angle * pi)
        #dy delta
        self.x += dy*cos(self.x_angle * pi + pi/2)
        self.y += dy*sin(self.x_angle * pi + pi/2)
        self.z += dz

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
        global camera
        camera = self
        self.loaded = True
        self.camera.load()

    def unload(self):
        self.loaded = False
        self.camera.unload()

    def world_projection(self):
        self.camera.world_projection()

    def hud_projection(self):
        self.camera.hud_projection()
