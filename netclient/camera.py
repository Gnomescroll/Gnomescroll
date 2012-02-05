import opts
opts = opts.opts

import init_c_lib
from math import sin, cos, pi

base_dir = "./"

def set_callback(callback):
    init_c_lib.camera_callback = callback

class Camera(object):

    _local = ['camera', 'name', 'loaded']

    def __init__(self, x=0.0, y=0.0, z=0.0, fov=85., first_person=False, name="free"):
        self.camera = init_c_lib.Camera(first_person=first_person, name=name)
        self.base_fov = fov
        self.zoom_factor = 2.
        self.camera.set_fov(fov)
        self.name = name
        self.x = x
        self.y = y
        self.z = z
        self.loaded = False
        self.zoomed = False

    def set_aspect(self, fov, z_near, z_far):
        self.camera.set_aspect(fov, z_near, z_far)

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

    def move_camera(self, dx, dy, dz):
        self.camera.move(dx,dy,dz)

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

    def forward(self):
        return self.camera.forward()
        
    def normal(self):
        return self.camera.normal()

    def toggle_zoom(self):
        zoom = not self.zoomed
        if zoom:
            self.zoom()
        else:
            self.unzoom()
            
    def zoom(self):
        self.zoomed = True
        self.camera.set_fov(self.fov / self.zoom_factor)

    def unzoom(self):
        self.zoomed = False
        self.camera.set_fov(self.base_fov)
