import opts
opts = opts.opts
import c_lib.c_lib_sdl
from math import sin, cos, pi

base_dir = "./"

def worldProjection():
    c_lib.c_lib_sdl.SDL_global.world_projection()

def hudProjection():
    c_lib.c_lib_sdl.SDL_global.hud_projection()

camera = None   # current active camera. Camera.load() will set this

class Camera(object):

    def __init__(self, x=0.0, y=0.0, z=0.0, rot=0.0, zoom=1.0, x_angle=0.0, y_angle=0.0, name=''):
        self.name = name
        self.x = x
        self.y = y
        self.z = z
        self.x_angle = x_angle
        self.y_angle = y_angle
        self.rot = rot
        self.zoom = zoom
        self.loaded = False
        
    def input_update(self):
        dxa, dya = c_lib.c_lib_input.get_mouse_deltas()
        self.pan(*self._convert_mouse_deltas(dxa,dya))
        self.set_c_camera()
        
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

    def pan(self, dx_angle, dy_angle):
        self.x_angle += dx_angle
        self.y_angle += dy_angle
        if self.y_angle < -0.499:
            self.y_angle = -0.499
        if self.y_angle > 0.499:
            self.y_angle = 0.499

    def pos(self, p=None):
        if p is None:
            return [self.x, self.y, self.z]
        else:
            self.x, self.y, self.z = p
        self.set_c_camera()
        
    def set_c_camera(self):
        c_lib.c_lib_sdl.set_projection(self.x,self.y,self.z,self.x_angle,self.y_angle)

    def angles(self, a=None):
        if a is None:
            return [self.x_angle, self.y_angle]
        else:
            self.x_angle, self.y_angle = a
        self.set_c_camera()

    def load(self):
        global camera
        camera = self
        self.set_c_camera()
        self.loaded = True

    def unload(self):
        self.loaded = False
