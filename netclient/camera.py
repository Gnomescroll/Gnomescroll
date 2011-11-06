import opts
opts = opts.opts
import c_lib.c_lib_sdl
from math import sin, cos, pi

base_dir = "./"

class Camera(object):

    def __init__(self, x=0.0, y=0.0, z=0.0, rot=0.0, zoom=1.0, x_angle=0.0, y_angle=0.0):
        self.x = x
        self.y = y
        self.z = z
        self.x_angle = x_angle
        self.y_angle = y_angle
        self.rot = rot
        self.zoom = zoom
        self.rts = True
        self.mode = 'camera'

    def worldProjection(self):
        ## swap buffers from last frame
        c_lib.c_lib_sdl.SDL_global.world_projection()

    def hudProjection(self):
        c_lib.c_lib_sdl.SDL_global.hud_projection()

    def update(self, poll_mouse=True):
        if poll_mouse:
            dxa, dya = c_lib.c_lib_input.get_mouse_deltas()
            self.pan(*self._convert_mouse_deltas(dxa,dya))
            c_lib.c_lib_sdl.set_projection(self.x,self.y,self.z,self.x_angle,self.y_angle)

        else:
            x,y,z = GameStateGlobal.agent.pos()
            xa,ya = GameStateGlobal.agent.angles()
            c_lib.c_lib_sdl.set_projection(x,y,z, xa,ya)
            
    def _convert_mouse_deltas(self, dx, dy):
        dx = (float(-dx) * opts.sensitivity) / 40000. # calibrated to sensitivity=100
        dy = (float(-dy) * opts.sensitivity) / 40000.
        return dx,dy

    def move_camera(self, dx, dy, dz):
        if self.rts:
            #dx delta
            self.x += dx*cos(self.x_angle * pi)
            self.y += dx*sin(self.x_angle * pi)
            #dy delta
            self.x += dy*cos(self.x_angle * pi + pi/2)
            self.y += dy*sin(self.x_angle * pi + pi/2)
        else:
            self.x += dx
            self.y += dy
        #dz delta
        self.z += dz

    def pan(self, dx_angle, dy_angle):
        self.x_angle += dx_angle
        self.y_angle += dy_angle
        if self.y_angle < -0.499:
            self.y_angle = -0.499
        if self.y_angle > 0.499:
            self.y_angle = 0.499

    def agent_view(self, agent):
        if self.mode != 'agent':
            self._save_position()
            self.mode = 'agent'
        self._load_position(agent)

    def camera_view(self):
        if self.mode != 'camera':
            self._load_position()
            self.mode = 'camera'

    def _save_position(self):
        self._x = self.x
        self._y = self.y
        self._z = self.z
        self._x_angle = self.x_angle
        self._y_angle = self.y_angle

    def _load_position(self, obj=None, prefix=''):
        if obj is None:
            obj = self
            prefix = '_'
        self.x = getattr(obj, prefix + 'x')
        self.y = getattr(obj, prefix + 'y')
        self.z = getattr(obj, prefix + 'z')
        if hasattr(obj, prefix + 'camera_height'):
            self.z += getattr(obj, prefix + 'camera_height')
        #self.x_angle = getattr(obj, prefix + 'x_angle')
        #self.y_angle = getattr(obj, prefix + 'y_angle')

from game_state import GameStateGlobal
