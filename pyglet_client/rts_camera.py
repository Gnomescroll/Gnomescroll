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


"""
A location/scale/angle, towards which a camera will quickly but smoothly
pan/zoom/tilt
"""
class Target(object):

    def __init__(self, camera):
		self.x, self.y, self.z = camera.x, camera.y, camera.z
		self.size = camera.scale
		self.x_angle = camera.x_angle
		self.y_angle = camera.y_angle


class RTS_Camera:
	
	def __init__(self, win_width, win_height, position=None, scale=None, x_angle=None, y_angle=None):
		print "ASFDSFDS"
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
		self.key_handlers = {}
		
		   # key.PAGEUP: lambda: self.zoom(2),
		   # key.PAGEDOWN: lambda: self.zoom(0.5),
		   # key.LEFT: lambda: self.pan(self.scale, -pi/2),
		   # key.RIGHT: lambda: self.pan(self.scale, +pi/2),
		   # key.DOWN: lambda: self.pan(self.scale, pi),
			#key.UP: lambda: self.pan(self.scale, 0),
			#key.COMMA: lambda: self.tilt(-1),
			#key.PERIOD: lambda: self.tilt(+1),
			#}
	
	def focus(self):
		pass
		"Set projection and modelview matrices ready for rendering"
        # Set projection matrix suitable for 2D rendering"
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		aspect = self.win_width / self.win_height
		gluPerspective( 45.0 / self.scale, aspect, 0.1, 100.0);
		
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity() 

		camera_focus_x = self.x + cos( self.x_angle * pi);
		camera_focus_y = self.y + sin( self.x_angle * pi);
		camera_focus_z = self.z + sin( self.y_angle);
		#print "Camera Angle: (%s, %s)" % (str(self.x_angle/(2*pi)), str(self.y_angle/(2*pi)))
		
		
		#print "Camera Focus Vector: (%s, %s, %s)" % ( str(cos( self.x_angle * pi)), str(sin( self.x_angle * pi)), str(sin( self.y_angle)))
		gluLookAt( self.x, self.y, self.z,
				camera_focus_x, camera_focus_y, camera_focus_z,
				0., 0., 1.0)
		        #gluOrtho2D(
        #    -self.scale * aspect,
        #    +self.scale * aspect,
        #    -self.scale,
        #    +self.scale)
       
	def zoom(self, factor):
		self.target.size *= factor

	def pan(self, dx_angle, dy_angle):
		#print "dx_angle= " + str(dx_angle)
		#print "dy_angle= " + str(dy_angle)
		self.x_angle += dx_angle
		self.y_angle += dy_angle

	def move_camera(self, dx, dy, dz):
		self.x += dx
		self.y += dy
		self.z += dz

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


	def hud_mode(self):
		"Set matrices ready for drawing HUD, like fps counter"
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		gluOrtho2D(0, self.win_width, 0, self.win_height)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()
