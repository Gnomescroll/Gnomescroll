import opts
opts = opts.opts

import init_c_lib
from math import sin, cos, pi

base_dir = "./"

def set_callback(callback):
    init_c_lib.camera_callback = callback

class Camera(init_c_lib.Camera):

    _local = ['name']

    def __init__(self, x=0.0, y=0.0, z=0.0, fov=85., name="free"):
        init_c_lib.Camera.__init__(self, name)
        self.set_fov(fov)
        self.name = name
        self.x = x
        self.y = y
        self.z = z
