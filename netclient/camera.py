from math import sin, cos, pi

import default_settings as settings

if settings.pyglet:
    from pyglet.gl import *
    from pyglet import clock, font, image, window
else:
    import SDL.gl

base_dir = "./"

class Camera(object):

    def __init__(self, win, x=0.0, y=0.0, z=0.0, rot=0.0, zoom=1.0, x_angle=0.0, y_angle=0.0):
        if settings.pyglet == False:
            self.SDL_global = SDL.gl.SDL_global
        if settings.pyglet:
            self.win = win
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
        if settings.pyglet:
            glMatrixMode(GL_PROJECTION)
            glLoadIdentity()
            aspect = self.win.width / float(self.win.height)
            #gluPerspective( 45.0 / self.scale, aspect, 0.1, 100.0);
            gluPerspective(65, aspect, .1, 1000)

            glMatrixMode( GL_MODELVIEW )
            glLoadIdentity()

            camera_focus_x = self.x + cos( self.x_angle * pi) * cos( self.y_angle * pi)
            camera_focus_y = self.y + sin( self.x_angle * pi) * cos( self.y_angle * pi)
            camera_focus_z = self.z + sin( self.y_angle)

            gluLookAt( self.x, self.y, self.z,
                    camera_focus_x, camera_focus_y, camera_focus_z,
                    0., 0., 1.0)

            glEnable (GL_DEPTH_TEST)
            #glEnable(GL_CULL_FACE);
        else:
            ## SDL prep
            #self.SDL_global.swap_buffers()  ##move to end of drawing frame ??
            ## swap buffers from last frame
            self.SDL_global.set_projection(self.x,self.y,self.z,self.x_angle,self.y_angle)
            self.SDL_global.world_projection()
            ## End SDL prep

    def hudProjection(self):

        if settings.pyglet:
            glMatrixMode(GL_PROJECTION)
            glLoadIdentity()
            gluOrtho2D(0, self.win.width, 0, self.win.height)

            glMatrixMode( GL_MODELVIEW )
            glLoadIdentity()

            glDisable(GL_DEPTH_TEST);
            #glDisable(GL_CULL_FACE);
            glEnable(gl.GL_TEXTURE_2D)
        else:
            self.SDL_global.hud_projection()

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

    def on_resize(self, width, height):
        if settings.pyglet:
            print "Resize Window"
            glViewport(0, 0, width, height)
            self.worldProjection()
            return pyglet.event.EVENT_HANDLED
        else:
            print "Deprecate Pyglet resize event"

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
            print getattr(obj, prefix + 'camera_height')
        self.x_angle = getattr(obj, prefix + 'x_angle')
        self.y_angle = getattr(obj, prefix + 'y_angle')
