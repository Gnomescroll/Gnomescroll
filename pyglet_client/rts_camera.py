"""
Camera tracks a position, orientation and zoom level, and applies openGL
transforms so that subsequent renders are drawn at the correct place, size
and orientation on screen
"""
from __future__ import division
from math import cos, pi, sin

from pyglet.window import key
from pyglet.gl import (
    glLoadIdentity, glMatrixMode, gluLookAt, gluPerspective, gluOrtho2D,
    GL_MODELVIEW, GL_PROJECTION,
)


class Target(object):
    """
    A location/scale/angle, towards which a camera will quickly but smoothly
    pan/zoom/tilt
    """
    def __init__(self, camera):
		self.x, self.y, self.z = camera.x, camera.y, camera.z
		self.size = camera.scale
		self.x_angle = camera.x_angle
		self.y_angle = camera.y_angle

class RTS_Camera(object):

    def __init__(self,
        win_width, win_height,
        position=None, scale=None, x_angle=None, y_angle=None
    ):
        self.win_width = win_width
        self.win_height = win_height
        if position is None:
            position = (0, 0, 0)
        self.x, self.y, self.z = position
        if scale is None:
            scale = 1
        self.scale = scale
        if x_angle is None:
            x_angle = 0
        self.x_angle = x_angle
        if y_angle is None:
            y_angle = 0
        self.y_angle = y_angle
        self.target = Target(self)
        self.key_handlers = {
           # key.PAGEUP: lambda: self.zoom(2),
           # key.PAGEDOWN: lambda: self.zoom(0.5),
           # key.LEFT: lambda: self.pan(self.scale, -pi/2),
           # key.RIGHT: lambda: self.pan(self.scale, +pi/2),
           # key.DOWN: lambda: self.pan(self.scale, pi),
            #key.UP: lambda: self.pan(self.scale, 0),
            #key.COMMA: lambda: self.tilt(-1),
            #key.PERIOD: lambda: self.tilt(+1),
        }


    def zoom(self, factor):
        self.target.size *= factor

    def pan(self, length, angle):
        self.target.x += length * sin(angle + self.angle)
        self.target.y += length * cos(angle + self.angle)

#    def tilt(self, angle):
#        self.target.angle += angle

#    def look_at(self, creature):
#       self.target = creature

    def update_position(self):
        "Quickly but smoothly move/zoom/tilt towards our target"
        self.x += (self.target.x - self.x) * 0.1
        self.y += (self.target.y - self.y) * 0.1
        self.z += (self.target.z - self.z) * 0.1
        self.scale += (self.target.size * 2 - self.scale) * 0.1
        self.x_angle += (self.target.x_angle - self.x_angle) * 0.1
        self.y_angle += (self.target.y_angle - self.y_angle) * 0.1

    def focus(self):
        "Set projection and modelview matrices ready for rendering"

        # Set projection matrix suitable for 2D rendering"
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        aspect = self.win_width / self.win_height
        #gluOrtho2D(
        #    -self.scale * aspect,
        #    +self.scale * aspect,
        #    -self.scale,
        #    +self.scale)
        gluPerspective( 45.0, aspect, 0.1, 100.0);
		
        # Set modelview matrix to move, scale & rotate to camera position"
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        gluLookAt(
            self.x, self.y, +1.0,
            self.x, self.y, -1.0,
            0, 0, 1.0)     
      
        #gluLookAt(
        #    self.x, self.y, +1.0,
        #    self.x, self.y, -1.0,
        #    sin(self.angle), cos(self.angle), 0.0)


	def hud_mode(self):
		"Set matrices ready for drawing HUD, like fps counter"
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		gluOrtho2D(0, self.win_width, 0, self.win_height)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()
